#include "../includes/client.hpp"
#include <ios>
#include <sstream>
#include <string>
#include <unistd.h>

void  swap_request(Request &a, Request &b) {
  Request tmp;

  tmp = a;
  a = b;
  b = tmp;
}

void  swap_clients(Client &a, Client &b) {
  Client tmp;

  tmp = a;
  a = b;
  b = tmp;
}

void  Request::set_error(int code, const char *s1, int line) {
  if (!header_code) {
    header_code = code;
    PRINT_ERR(to_string(s1) + " : " + to_string(line));
  }

}

void  Request::get_header(std::string &request, Client &parent, Client &tmp) {

  std::cout << "____________________________________________" << std::endl << std::endl;

  std::istringstream stream(request, std::ios_base::binary | std::ios_base::out);
  size_t  line_count = 0;

  parent._fav = false;
  if (method.size() < 1) {
    stream >> method;
    stream >> path;
    stream >> version;
    
    std::cout << method << " " << path << " " << version << std::endl;
    std::istringstream tmpstream(path, std::ios_base::binary | std::ios_base::out);

    if (path.size() > 150)
      set_error(414);
    std::getline(tmpstream, path, '?');
    std::getline(tmpstream, query);
    parent._path = path;
    if (comp(path, ".ico") == true)
      parent._fav = true;
    path_info = path;
    if (comp(path, "cgi")) {
      size_t pos = find(path, '/', 3);
        using_cgi = true;
        pos++;
        path_info = path.substr(pos - 1);
        path = path.substr(0, pos - 1);
    }
    else if (comp(path, "errors/style.css")) {
      path = "/errors/style.css";
      path_info = path;
    }

  }
  while (std::getline(stream, sline)) {
    if (sline.size() == 1 && line_count == 0) {
      continue ;
    }
    else if (sline == "\r") {
      complete_header = true;
      break ;
    }
    std::cout << sline << std::endl;
    if (sline.size()) {
      std::istringstream line_stream(sline);
      std::getline(line_stream, key, ':');
      if (key.size() < 1 || (key.size() >= sline.size() - 1))
        break;
      std::getline(line_stream, value);
    }
    else
      break;
    if (header.find(key) != header.end()) {
      set_error(400);
    }
    header[key] = value;
    if (value.size()) {
      if (comp(key, "Content-Length") == true) {
        content_lenght = atoi(value.c_str());
        initial_lenght = content_lenght;
        body_size = content_lenght;
        has_size = true;
        has_body = true;
      }
      else if (comp(key, "Content-type") == true) {
        std::istringstream value_stream(value);
        std::getline(value_stream, type, ';');
        std::getline(value_stream, boundary, '=');
        std::getline(value_stream, boundary);
        //if (content_lenght < 1)
          //content_lenght = 1;
        has_body = true;
      }
      else if (comp(key, "Host") == true) {
        host = trim(value);
      }
      else if (comp(key, "Cookie") == true && parent._fav == false) {
        auth = true;
        parent._cookie = value;
        cookie = value;
      }
      else if (comp(key, "Content-Type: multipart/form-data") == true) {
        std::istringstream bound(value);
        std::getline(bound, key, '=');
        if (bound.rdbuf()->in_avail() < 1)
          break;
        std::getline(bound, boundary);
      }
      else if (comp(key, "transfer-encoding") == true) {
        if (comp(value, "chunked") == true) {
          PRINT_LOG("Chunked");
          transfer_encoding = "chunked";
        }
      }
    }
    line_count++;
  }
  if (host.size()) {
    int hcount = 0;
    for (size_t l = 0; l < host.size(); l++) {
      if (host[l] == ':') {
        hcount++;
        if (l == host.size() - 1)
            set_error(400);
        if (hcount > 1)
          break;
      }
      else if (hcount == 0 && !isalnum(host[l]) && host[l] != '.') {
        set_error(400);
        break;
      }
      else if (hcount == 1 && !isdigit(host[l])) {
        set_error(400);
        break;
      }
    }
    if (hcount != 1)
      set_error(400);
    parent._host = host;
  }
  else
    set_error(400);
  if (has_body && initial_lenght == 0 && transfer_encoding != "chunked")
    set_error(411);
  if (comp(method, "delete"))
    has_body = false;
  if (comp(method, "post") == false && has_body == true) {
      set_error(400);
  }
  if (header_code != 0)
    return ;
  PRINT_LOG("phost:" + parent._host);
  PRINT_LOG("host:" + host);
  if (parent._host.size() && parent._host != host) {
    PRINT_ERR("DISCONNECTING USER: " + parent._name);
    parent._log = false;
    parent._lastname = "Each server has it's own login system, current client got disconnected";
  }
  parent._host = host;
  if (has_body == false)
    complete_header = true;
  if (complete_header == true && has_body)
    get_body_stream(stream, parent, tmp);
}

void  Request::get_body_stream(std::istringstream &stream, Client &parent, Client &tmp) {
  std::vector<unsigned char> buff(buff_size);
  int int_bytes = 0;
  if (comp(transfer_encoding, "chunked") == false)
  {
    if (content_lenght > 0 && has_size == true) {
      int old_bytes = stream.gcount();
      if (content_lenght >= sizeof(buff))
        stream.read(reinterpret_cast<char *>(&buff[0]), buff_size - 1);
      else
        stream.read(reinterpret_cast<char *>(&buff[0]), content_lenght);
      int_bytes = stream.gcount() - old_bytes;
      current_bytes = int_bytes;
      for (int i = 0; i < int_bytes; i++) {
        if (buff[i] == '\n') {
          ncount++;
        }
        body_str.push_back(buff[i]);
      }
      content_lenght -= current_bytes;
    }
    else if (has_body == true && has_size == false) {
      int old_bytes = stream.gcount();
      stream.read(reinterpret_cast<char *>(&buff[0]), buff_size - 1);
      int_bytes = stream.gcount() - old_bytes;
      current_bytes = int_bytes;
      if (int_bytes < 1 || current_bytes < buff_size - 1) {
        content_lenght = 0;
      }
      for (size_t i = 0; i < buff.size(); i++) {
        if (buff[i] == '\n')
          ncount++;
        body_str.push_back(buff[i]);
      }
    }
    if (comp(type, "x-www-form-urlencoded")) {
      query = body_str;
      std::istringstream line_stream(body_str);
      std::string key;
      std::string value;
      while (std::getline(line_stream, key, '=')) {
        if (line_stream.rdbuf()->in_avail() < 1)
            break;
        std::getline(line_stream, value, '&');
        body[key] = value;
        if (comp(key, "fname")) {
          if (parent._name.size()) {
            tmp = parent;
            tmp._sock = 0;
            parent._files.clear();
            parent._oldname = parent._name;
          }
          parent._name = value;
          parent._log = true;
        }
        else {
          parent._lastname = value;
        }
      }
    }
  }
  /*else {
  PRINT_LOG("Reading Chunked body");
  std::string tmp;
  int         chunk_size = 0;

  while ((int_bytes = read(client, buff, 1)) > 0) {
  if (!(isdigit(buff[0]) || buff[0] == '\r' || buff[0] == '\n')) {
  PRINT_ERR("Invalid chunk size format");
  exit(0);
  }
  tmp += buff[0];
  if (erase(tmp, "\r\n") == true)
  break ;
  chunk_size++;
  if (chunk_size > 6) {
  PRINT_ERR("Excessive chunk size");
  exit(0);
  }
  }
  chunk_size = atoi(tmp.c_str());
  size_t  compchunk = chunk_size;
  if (compchunk >= sizeof(buff)) {
  PRINT_ERR("Chunk size too big compared to buff size");
  exit(0);
  }
  if ((int_bytes = read(client, buff, chunk_size)) > 0)
  tmp = buff;
  if (int_bytes == -1) {
  PRINT_ERR("Couldn't read from client");
  exit(0);
  }
  if (tmp == "\r\n") {
  content_lenght = 0;
  }
  else
  body_str += tmp;
  }*/
}

void  Request::get_body(int client) {
  std::vector<unsigned char> buff(buff_size + 1);

  int int_bytes = 0;
  if (comp(transfer_encoding, "chunked") == false)
  {
    if (content_lenght > 0 && has_size == true) {
      int_bytes = recv(client, &buff[0], buff_size - 1, 0);
      if (int_bytes < 1)
        return ;
      current_bytes = int_bytes;
      for (int i = 0; i < int_bytes; i++) {
        if (buff[i] == '\n') {
          ncount++;
          fpos = body_str.size();
        }
        body_str.push_back(buff[i]);
      }
      content_lenght -= current_bytes;
    }
    else if (has_body == true && has_size == false) {
      int_bytes = recv(client, &buff[0], buff_size - 1, 0);
      current_bytes = int_bytes;
      if (int_bytes < 1 || current_bytes < buff_size) {
        complete_body = true;
        content_lenght = 0;
      }
      for (int i = 0; i < int_bytes; i++)
        body_str.push_back(buff[i]);
    }
  }
  else {
    std::string tmp;
    int         chunk_size = 0;

    while ((int_bytes = recv(client, &buff[0], 1, 0)) > 0) {
      if (!(isdigit(buff[0]) || buff[0] == '\r' || buff[0] == '\n')) {
        set_error(400);
      }
      tmp += buff[0];
      if (erase(tmp, "\r\n") == true)
        break ;
      chunk_size++;
      if (chunk_size > 6) {
        set_error(400);
      }
    }
    chunk_size = atoi(tmp.c_str());
    size_t  compchunk = chunk_size;
    if (compchunk >= buff_size - 1) {
      set_error(400);
    }
    tmp.clear();
    if ((int_bytes = recv(client, &buff[0], chunk_size, 0)) > 0)
      for (size_t i = 0; i < buff.size(); i++)
        tmp += buff[i];
    if (int_bytes == -1) {
      set_error(400);
    }
    if (tmp == "\r\n") {
      complete_body = true;
      content_lenght = 0;
    }
    else
      body_str += tmp;
  }
}

std::string Request::check_method(std::string &method) {
  if (method == "GET" || method == "POST" || method == "DELETE") {
    return std::string("HTTP/1.1 200 OK");
  }
  if (method == "HEAD" || method == "PUT" || method == "CONNECT"
      || method == "OPTIONS" || method == "TRACE" || method == "PATCH") {
    set_error(501);
    return std::string("HTTP/1.1 501 Not Implemented");
  }
  set_error(400);
  return std::string ("HTTP/1.1 400 Bad Request");
}

int Request::parse_header(void) {
  std::string method_buffer;

  if (method.empty()) {
    set_error(400);
    return -1;
  }
  method_buffer = check_method(method);
  status = method_buffer + "\n";
  if (method_buffer.size() > strlen("HTTP/1.1 200 OK ")) {
    return -1;
  }
  if (method == "POST" && has_body == false) {
    status = "HTTP/1.1 400 Bad Request";
    set_error(400);
    return -1;
  }
  if (!host.size()) {
    status = "HTTP/1.1 400 Bad Request";
    set_error(400);
    return -1;
  }
  parsed_header = true;
  return 0;
}

int  Request::parse_body(Client &parent) {
  if (boundary.size()) {
    size_t pos;
    if ((pos = body_str.find(boundary)) != std::string::npos) {
      Body tmpbody;
      std::string cpy(body_str, pos);
      cpy.erase(cpy.size() - 1, 1);
      std::istringstream bodystream(cpy, std::ios_base::binary | std::ios_base::out);
      body_str.clear();
      int boundary_count = 0;
      std::ofstream file;
      std::string server_name = host.substr(0, host.find(":"));
      while (std::getline(bodystream, cpy)) {
        int old_boundary_count = boundary_count;
        if (cpy.find("Content-Disposition") != std::string::npos) {
          size_t found;
          tmpbody.disposition = cpy;
          found = tmpbody.disposition.find_last_of('=');
          tmpbody.filename = tmpbody.disposition.substr(found + 1);
          erase(tmpbody.filename);
          tmpbody.filename.erase(tmpbody.filename.size() - 1, 1);
          if ( parent._name.size())
          {
            std::string file_path("/tmp/private_webserv/" + server_name + "/" + parent._name);
            struct stat st;

            if ( stat(file_path.c_str(), &st) == -1 )
              if (mkdir(file_path.c_str(), 0777) == -1) {
                set_error(500);
                parsed_body = true;
                return -1;
              }

            file_path += "/";
            file_path += tmpbody.filename.c_str();
            file.open(file_path.c_str(), std::ios::binary);
          }
          else
          {
            std::string file_path("/tmp/private_webserv/" + server_name + "/" + "unknown");
            struct stat st;

            if ( stat(file_path.c_str(), &st) == -1 )
              if (mkdir(file_path.c_str(), 0777) == -1) {
                set_error(500);
                parsed_body = true;
                return -1;
              }
            file_path += "/";
            file_path += tmpbody.filename;
            file.open(file_path.c_str(), std::ios::binary);
          }
          continue;
        }
        else if (cpy.find("Content-Type") != std::string::npos) {
          tmpbody.type = cpy;
          continue;
        }
        if ((cpy.size() < boundary.size() + 6) && comp(cpy, boundary) == true) {
          boundary_count++;
        }
        else {
          bool  end = false;
          if (cpy[0] == 13 && linecount == 0)
            continue;
          if (bodystream.peek() == 45) {
            std::string test;
            std::streampos  save = bodystream.tellg();
            std::getline(bodystream, test);
            if ((test.size() < boundary.size() + 6) && comp(test, boundary) == true) {
              end = true;
            }
            bodystream.seekg(save);
          }
          if (end == false)
            file << cpy << "\n";
          else {
            cpy.erase(cpy.size() - 1, 1);
            file << cpy;
          }
          linecount++;
          tmpbody.data.push_back(cpy);
        }
        if (boundary_count == 2 && boundary_count != old_boundary_count) {
          multi_body.push_back(tmpbody);
          file.close();
          parent._files.push_back(tmpbody.filename);
          tmpbody.clear();
        }
        else if (boundary_count > 2 && boundary_count % 2 == 1 && old_boundary_count != boundary_count) {
          multi_body.push_back(tmpbody);
          file.close();
          parent._files.push_back(tmpbody.filename);
          tmpbody.clear();
        }
      }
      if (boundary_count < 2 || (boundary_count > 2 && boundary_count % 2 != 1 && boundary_count)) {
        set_error(400);
        status = "400 Bad Request";
        return -1;
      }
    }
    else {
      set_error(400);
      status = "400 Bad Request";
      return -1;
    }
  }
  else if (comp(type, "form-urlencoded") == true) {
    std::istringstream bodystream(body_str);
    std::string line;
    while (std::getline(bodystream, line)) {
      std::istringstream line_stream(line);
      while (std::getline(line_stream, key, '=')) {
        if (line_stream.rdbuf()->in_avail() < 1)
          break ;
        std::getline(line_stream, value, '&');
        body[key] = value;
      }
    }
  }
  parsed_body = true;
  return 0;
}

void  Request::clear(void) {
  fpos = 0;
  ncount = 0;
  linecount = 0;
  initial_lenght = 0;
  auth_redirect = 0;
  complete_body = false;
  auth = false;
  has_size = false;
  in_use = false;
  answer = "HTTP/1.1 ";
  content_lenght = 0;
  header_code = 0;
  bytes = 0;
  current_bytes = 0;
  has_body = false;
  complete_header = false;
  parsed_body = false;
  parsed_header = false;
  sline.clear();
  method.clear();
  path.clear();
  version.clear();
  host.clear();
  index.clear();
  type.clear();
  key.clear();
  value.clear();
  boundary.clear();
  header.clear();
  body.clear();
  body_str.clear();
  multi_body.clear();
  file_content.clear();
  file_path.clear();
  file_type.clear();
  path_info.clear();
  query.clear();
  cgi.clear();
  cgi_path.clear();
  link.clear();
  using_cgi = false;
  chunked = false;
  nread = 0;
  cgi_size = 0;
  file_size = 0;
  read_size = 0;
  complete_file = false;
  in_response = false;
  read_count = 0;
  found_user = false;
  auto_index = false;
}

int  Request::read_client(int client, Client &parent, Client &tmp) {
  std::string request;
  int int_bytes = 0;

  std::vector<unsigned char> buff(buff_size);

  if (in_response == true) {
    return 1;
  }
  if (parsed_body == true && parsed_header == true) {
    return 1;
  }
  if (complete_header == false) {
    if ((int_bytes = recv(client, &buff[0], buff.size() - 1, 0)) > 0) {
      current_bytes = int_bytes;
      bytes += current_bytes;
      for (int i = 0; i < int_bytes; i++)
        request += buff[i];
    }
    if (int_bytes < 0) {
      return -1;
    }
    else if (int_bytes == 0) {
      return 0;
    }
    in_use = true;
    get_header(request, parent, tmp);
    if (header_code != 0) {
      in_use = false;
      complete_header = true;
      parsed_header = true;
      parsed_body = true;
      complete_file = true;
      return 1;
    }
  }
  if (complete_header == true && has_body)
    get_body(client);
  if (complete_header && parsed_header == false)
    parse_header();
  if (complete_header && has_body && content_lenght < 1 && parsed_body == false)
    parse_body(parent);
  if (complete_header && has_body == false)
    parsed_body = true;
  if (parsed_body == true && parsed_header == true) {
    in_use = false;
    return 1;
  }
  if (parsed_body == true) {
    in_use = false;
    return 1;
  }
  return 2;
}

Request::Request(void) {
  linecount = 0;
  fpos = 0;
  initial_lenght = 0;
  ncount = 0;
  auth_redirect = 0;
  content_lenght = 0;
  complete_body = false;
  header_code = 0;
  auth = false;
  has_size = false;
  in_use = false;
  answer = "HTTP/1.1 ";
  has_body = false;
  complete_header = false;
  parsed_body = false;
  parsed_header = false;
  bytes = 0;
  current_bytes = 0;
  body_size = 0;

  sline.clear();
  method.clear();
  path.clear();
  version.clear();
  host.clear();
  index.clear();
  cookie.clear();
  URI.clear();
  transfer_encoding.clear();
  type.clear();
  key.clear();
  value.clear();
  boundary.clear();
  link.clear();
  name.clear();
  status.clear();
  content_type.clear();
  file_content.clear();
  file_path.clear();
  file_type.clear();
  auto_index = false;
  file_size = 0;
  read_size = 0;
  complete_file = false;
  in_response = false;
  read_count = 0;
  found_user = false;
  path_info.clear();
  query.clear();
  cgi.clear();
  cgi_path.clear();
  using_cgi = false;
  chunked = false;
  nread = 0;
  cgi_size = 0;
  header.clear();
  body.clear();
  body_str.clear();
  multi_body.clear();
  answer.clear();
}

Request::~Request(void) {
}

Request::Request(std::string &request) {
  (void)request;
}

std::ostream &operator<<(std::ostream &n, Request &req) {
  n << std::endl << "\033[7mRequest data\033[0m" << std::endl << std::endl
    << "\033[1m\033[2mMethod:\033[0m" << req.method << std::endl
    << "\033[1m\033[2mPath:\033[0m" << req.path << std::endl
    << "\033[1m\033[2mPath info:\033[0m" << req.path_info << std::endl
    << "\033[1m\033[2mQuery:\033[0m" << req.query << std::endl
    << "\033[1m\033[2mHost:\033[0m" << req.host << std::endl
    << "\033[1m\033[2mCookie:\033[0m" << req.cookie << std::endl
    << "\033[1m\033[2mType:\033[0m" << req.type << std::endl
    << "\033[1m\033[2mCode:\033[0m" << req.header_code << std::endl
    << "\033[1m\033[2mBoundary:\033[0m" << req.boundary << std::endl << std::endl
    << "\033[1m\033[2mHas body:\033[0m" << std::boolalpha << req.has_body << std::endl
    << "\033[1m\033[2mHas size:\033[0m" << std::boolalpha << req.has_body << std::endl
    << "\033[1m\033[2mBody size:\033[0m" << req.body_size << std::endl
    << "\033[1m\033[2mHeader complete:\033[0m" << std::boolalpha << req.complete_header << std::endl
    << "\033[1m\033[2mParsed header:\033[0m" << std::boolalpha << req.parsed_header << std::endl
    << "\033[1m\033[2mParsed body:\033[0m" << std::boolalpha << req.parsed_body << std::endl << std::endl;

  n << "\033[1m\033[2mHeader:\033[0m" << std::endl;
  for (std::map<std::string, std::string>::iterator it = req.header.begin(); it != req.header.end(); it++) {
    n << "    \033[1m" << it->first << "\033[0m     \033[38;5;110m" << it->second << "\033[0m" << std::endl;
  }
  n << std::endl << "\033[1m\033[2mBody:\033[0m" << std::endl;
  for (std::map<std::string, std::string>::iterator it = req.body.begin(); it != req.body.end(); it++) {
    n << "    \033[1m" << it->first << "\033[0m     \033[38;5;110m" << it->second << "\033[0m" << std::endl;
  }
  n << std::endl << "\033[1m\033[2mMultibody:\033[0m" << std::endl;
  size_t count = 0;
  for (std::vector<Body>::iterator it = req.multi_body.begin(); it != req.multi_body.end(); it++) {
    //std::ofstream file(it->filename, std::ios::binary);
    n << "\033[1;35mSub body " << count << "\033[0m" << std::endl << std::endl
      << "\033[1mdisposition:\033[0m\033[38;5;110m " << it->disposition << "\033[0m" << std::endl
      << "\033[1mfilename:\033[0m\033[38;5;110m " << it->filename << "\033[0m" << std::endl
      << "\033[1mtype:\033[0m\033[38;5;110m " << it->type << "\033[0m" << std::endl
      << "\033[1mdata:\033[0m " << std::endl;
    for (std::vector<std::string>::iterator sit = it->data.begin(); sit != it->data.end(); sit++) {
      n << "  \033[38;5;223m" << *sit << "\033[0m" << std::endl;
      // file << *sit << "\n";
    }
    //file.close();
    count++;
    n << std::endl;
  }
  n << "\033[1m\033[2mBody str:\033[0m" << req.body_str << std::endl;
  // n << std::endl << "\033[1m\033[2mAnswer:\033[0m" << req.answer << std::endl << std::endl;
  return n;
}
