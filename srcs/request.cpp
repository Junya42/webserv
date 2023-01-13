#include "../includes/client.hpp"
#include <sstream>
#include <unistd.h>

void  Request::set_error(int code) {
  if (!header_code)
    header_code = code;
}

void  Request::get_header(std::string &request, Client &parent, Client &tmp) {

  std::istringstream stream(request, std::ios_base::binary | std::ios_base::out);
  std::string line;
  size_t  line_count = 0;

  PRINT_FUNC();
  std::cout << "_____________________________" << std::endl
    << "\033[1;32mRequest: \033[0m" << std::endl
    << request << std::endl;

  parent._fav = false;
  if (method.size() < 1) {
    stream >> method >> path >> version;
    parent._path = path;
    if (comp(path, ".ico") == true)
      parent._fav = true;
  }
  PRINT_LOG(method);
  PRINT_LOG(path);
  PRINT_LOG(version);
  while (std::getline(stream, line)) {
    if (line == "\r" && line_count == 0)
      continue ;
    else if (line == "\r") {
      PRINT_WIN("Complete header");
      complete_header = true;
      break ;
    }
    std::istringstream line_stream(line);
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);

    header[key] = value;
   // PRINT_LOG(std::string(key + " : " + value, 0, key.size() + value.size() + 2));
    if (comp(key, "Content-Length") == true) {
      content_lenght = atoi(value.c_str());
      PRINT_LOG("Content lenght = " + to_string(content_lenght));
      body_size = content_lenght;
      has_size = true;
      has_body = true;
    }
    else if (comp(key, "Content-type") == true) {
      std::istringstream value_stream(value);
      std::getline(value_stream, type, ';');
      std::getline(value_stream, boundary, '=');
      std::getline(value_stream, boundary);
      //type = value;
      if (content_lenght < 1)
        content_lenght = 1;
      has_body = true;
    }
    else if (comp(key, "Host") == true) {
      host = value;
    }
    else if (comp(key, "Cookie") == true && parent._fav == false) {
      auth = true;
      parent._cookie = value;
      cookie = value;
    }
    else if (comp(key, "Content-Type: multipart/form-data") == true) {
      std::istringstream bound(value);
      std::getline(bound, key, '=');
      std::getline(bound, boundary);
    }
    else if (comp(key, "transfer-encoding") == true) {
      transfer_encoding = value;
    }
    line_count++;
  }
  std::cout << "_____________________________" << std::endl;
  PRINT_LOG("After header Content lenght = " + to_string(content_lenght));
  //std::cout << *this << std::endl;
  if (complete_header == true && has_body)
    get_body_stream(stream, parent, tmp);
}

void  Request::get_body_stream(std::istringstream &stream, Client &parent, Client &tmp) {
  PRINT_FUNC();
  //char buff[buff_size];
  //memset(buff, 0, buff_size);
  std::vector<unsigned char> buff(buff_size);
  int int_bytes = 0;
  if (comp(transfer_encoding, "chunked") == false)
  {
    PRINT_LOG("Body not chunked");
    PRINT_LOG(content_lenght);
    PRINT_LOG(has_size);
    if (content_lenght > 0 && has_size == true) {
      int old_bytes = stream.gcount();
      if (content_lenght > sizeof(buff))
        stream.read(reinterpret_cast<char *>(&buff[0]), buff_size);
      else
        stream.read(reinterpret_cast<char *>(&buff[0]), content_lenght);
      PRINT_WIN(&buff[0]);
      int_bytes = stream.gcount() - old_bytes;
      current_bytes = int_bytes;
      PRINT_LOG("Reading body with content_lenght: " + std::to_string(content_lenght));
      PRINT_LOG("bytes read: " + std::to_string(int_bytes));
      //body_str += buff;
      for (size_t i = 0; i < buff.size(); i++)
        body_str.push_back(buff[i]);
      content_lenght -= current_bytes;
    }
    else if (has_body == true && has_size == false) {
      int old_bytes = stream.gcount();
      stream.read(reinterpret_cast<char *>(&buff[0]), buff_size);
      int_bytes = stream.gcount() - old_bytes;
      PRINT_LOG("Reading body without content_lenght: " + std::to_string(content_lenght));
      PRINT_LOG("bytes read: " + std::to_string(int_bytes));
      current_bytes = int_bytes;
      std::cout << "sizeof buff: " << buff_size << std::endl;
      std::cout << "current_bytes: " << int_bytes << std::endl;
      if (int_bytes < 1 || current_bytes < buff_size) {
        content_lenght = 0;
      }
      //body_str += buff;
      for (size_t i = 0; i < buff.size(); i++)
        body_str.push_back(buff[i]);
    }
    PRINT_LOG("TYPE = " + type);
    if (comp(type, "x-www-form-urlencoded")) {
      PRINT_LOG("Setting names");
      std::istringstream line_stream(body_str);
      std::string key;
      std::string value;
      PRINT_LOG(body_str);
      while (std::getline(line_stream, key, '=')) {
        std::getline(line_stream, value, '&');
        body[key] = value;
        PRINT_LOG(key);
        if (comp(key, "fname")) {
          if (parent._name.size()) {
            PRINT_LOG("Creating new client based on parent");
            tmp = parent;
            tmp._sock = 0;
            parent._oldname = parent._name;
          }
          parent._name = value;
          PRINT_WIN("Set client name to: " + value);
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
  PRINT_FUNC();
  //char buff[buff_size];
  std::vector<unsigned char> buff(buff_size);

  //memset(buff, 0, buff_size);
  int int_bytes = 0;
  if (comp(transfer_encoding, "chunked") == false)
  {
    PRINT_LOG("Body not chunked");
    PRINT_LOG(content_lenght);
    PRINT_LOG(has_size);
    if (content_lenght > 0 && has_size == true) {
      //int_bytes = read(client, buff, sizeof(buff));
      int_bytes = read(client, &buff[0], buff_size);
      current_bytes = int_bytes;
      PRINT_LOG("Reading body with content_lenght");
      PRINT_LOG(int_bytes);
      for (size_t i = 0; i < buff.size(); i++)
        body_str += buff[i];
      //body_str += buff;
      content_lenght -= current_bytes;
    }
    else if (has_body == true && has_size == false) {
      //int_bytes = read(client, buff, sizeof(buff));
      int_bytes = read(client, &buff[0], buff_size);
      PRINT_LOG("Reading body without content_lenght");
      current_bytes = int_bytes;
      std::cout << "sizeof buff: " << buff_size << std::endl;
      std::cout << "current_bytes: " << int_bytes << std::endl;
      if (int_bytes < 1 || current_bytes < buff_size) {
        content_lenght = 0;
      }
      for (size_t i = 0; i < buff.size(); i++)
        body_str += buff[i];
      //body_str += buff;
    }
  }
  else {
    PRINT_LOG("Reading Chunked body");
    std::string tmp;
    int         chunk_size = 0;

    //while ((int_bytes = read(client, buff, 1)) > 0) {
    while ((int_bytes = read(client, &buff[0], 1)) > 0) {
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
    if (compchunk >= buff_size) {
      PRINT_ERR("Chunk size too big compared to buff size");
      exit(0);
    }
    tmp.clear(); //ADDITION WITH VECTOR BUFF
    //if ((int_bytes = read(client, buff, chunk_size)) > 0)
    if ((int_bytes = read(client, &buff[0], chunk_size)) > 0)
      for (size_t i = 0; i < buff.size(); i++)
        tmp += buff[i];
      //tmp = buff;
    if (int_bytes == -1) {
      PRINT_ERR("Couldn't read from client");
      exit(0);
    }
    if (tmp == "\r\n") {
      content_lenght = 0;
    }
    else
      body_str += tmp;
  }
}

std::string Request::check_method(std::string &method) {
  PRINT_FUNC();
  if (method == "GET" || method == "POST" || method == "DELETE") {
    PRINT_WIN("Success");
    return std::string("HTTP/1.1 200 OK");
  }
  if (method == "HEAD" || method == "PUT" || method == "CONNECT" || method == "OPTIONS" || method == "TRACE" || method == "PATCH") {
    PRINT_ERR("501 Not Implemented");
    set_error(501);
    return std::string("HTTP/1.1 501 Not Implemented");
  }
    set_error(400);
  PRINT_ERR("400 Bad Request");
  return std::string ("HTTP/1.1 400 Bad Request");
}

int Request::parse_header(void) {
  PRINT_FUNC();
  std::string method_buffer;

  method_buffer = check_method(method);
  status = method_buffer + "\n";
  if (method_buffer.size() > strlen("HTTP/1.1 200 OK ")) {
    PRINT_ERR(method_buffer);
    return -1;
  }
  if (method == "POST" && has_body == false) {
    PRINT_ERR("Couldn't find body in POST request");
    status = "HTTP/1.1 400 Bad Request";
    set_error(400);
    return -1;
  }
  if (!host.size()) {
    PRINT_ERR("Couldn't find host");
    status = "HTTP/1.1 400 Bad Request";
    set_error(400);
    return -1;
  }
  parsed_header = true;
  PRINT_WIN("Success");
  return 0;
}

int  Request::parse_body(void) {
  PRINT_FUNC();
  if (boundary.size()) {
    PRINT_LOG("Found boundary in header");
    size_t pos;
    if ((pos = body_str.find(boundary)) != std::string::npos) {
      Body tmpbody;
      PRINT_LOG(body_str);
      std::string cpy(body_str, pos);
      std::istringstream bodystream(cpy, std::ios_base::binary | std::ios_base::out);
      body_str.clear();
      int boundary_count = 0;
      int linecount = 0;
      std::ofstream file;
      while (std::getline(bodystream, cpy)) {
        int old_boundary_count = boundary_count;
        PRINT_LOG(cpy);
        if (cpy.find("Content-Disposition") != std::string::npos) {
          size_t found;
          tmpbody.disposition = cpy;
          found = tmpbody.disposition.find_last_of('=');
          tmpbody.filename = tmpbody.disposition.substr(found + 1);
          //erase(tmpbody.filename, '"');
          erase(tmpbody.filename);
          file.open(tmpbody.filename.c_str(), std::ios::binary);
          continue;
        }
        else if (cpy.find("Content-Type") != std::string::npos) {
          tmpbody.type = cpy;
          continue;
        }
        if (comp(cpy, boundary) == true) {
          PRINT_WIN(boundary);
          PRINT_WIN("FOUND BOUNDARY");
          boundary_count++;
        }
        else {
          //PRINT_LOG("Adding cpy to file");
          //PRINT_LOG(static_cast<int>(cpy[0]));
          if (cpy[0] == 13 && linecount == 0)
            continue;
          file << cpy << "\n";
          linecount++;
          tmpbody.data.push_back(cpy);
        }
        if (boundary_count == 2 && boundary_count != old_boundary_count) {
          PRINT_WIN("PUSHBACK 1");
          multi_body.push_back(tmpbody);
          file.close();
          tmpbody.clear();
        }
        else if (boundary_count > 2 && boundary_count % 2 == 1 && old_boundary_count != boundary_count) {
          PRINT_WIN("PUSHBACK 2");
          multi_body.push_back(tmpbody);
          file.close();
          tmpbody.clear();
        }
      }
      if (boundary_count < 2 || (boundary_count > 2 && boundary_count % 2 != 1 && boundary_count)) {
        set_error(400);
        PRINT_ERR("Uneven Boundary count");
        status = "400 Bad Request";
        return -1;
      }
    }
    else {
      set_error(400);
      PRINT_ERR("Couldn't find Boundary in Body str");
      status = "400 Bad Request";
      return -1;
    }
  }
  else if (comp(type, "form-urlencoded") == true) { //DANGER DANGER //DANGER DANGER //DANGER DANGER //DANGER DANGER //DANGER DANGER //DANGER DANGER
                                                    PRINT_LOG("Parsing form body");
    std::istringstream bodystream(body_str);
    std::string line;
    while (std::getline(bodystream, line)) {
      std::istringstream line_stream(line);
      while (std::getline(line_stream, key, '=')) {
        std::getline(line_stream, value, '&');
        body[key] = value;
      }
    }
  }
  parsed_body = true;
  PRINT_WIN("Success");
  return 0;
}

void  Request::clear(void) {
  PRINT_FUNC();
  auth_redirect = false;
  auth = false;
  has_size = false;
  in_use = false;
  answer = "HTTP/1.1 ";
  header_code = 0;
  bytes = 0;
  current_bytes = 0;
  has_body = false;
  complete_header = false;
  parsed_body = false;
  parsed_header = false;
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
  file_size = 0;
  read_size = 0;
  complete_file = false;
  in_response = false;
  read_count = 0;
  found_user = false;
}

int  Request::read_client(int client, Client &parent, Client &tmp) {
  std::string request;
  int int_bytes = 0;
  //char buff[buff_size];
  name = parent._name;
  std::vector<unsigned char> buff(buff_size);

  PRINT_FUNC();
  if (in_response == true) {
    PRINT_LOG("Continuing getting response");
    return 1;
  }
  if (parsed_body == true && parsed_header == true) {
    PRINT_WIN("Success");
    return 1;
  }
  if (complete_header == false) {
    if ((int_bytes = read(client, &buff[0], buff.size())) > 0) {
      current_bytes = int_bytes;
      bytes += current_bytes;
      for (int i = 0; i < int_bytes; i++)
        request += buff[i];
    }
    if (int_bytes < 0) {
      PRINT_ERR("Error reading from client");
      return -1;
    }
    else if (int_bytes == 0) {
      PRINT_LOG("No more data in client fd");
      return 0;
    }
    in_use = true;
    get_header(request, parent, tmp);
  }
  if (complete_header == true && has_body)
    get_body(client);
  if (complete_header && parsed_header == false)
    parse_header();
  //if (complete_header && has_body && content_lenght < 1 && parsed_body == false)
  if (complete_header && has_body && parsed_body == false)
    parse_body();
  if (complete_header && has_body == false)
    parsed_body = true;
  if (parsed_body == true && parsed_header == true) {
    in_use = false;
    PRINT_WIN("Success");
    return 1;
  }
  PRINT_LOG("Uncomplete request or bad request");
  std::cout << *this << std::endl;
  sleep(2);
  //PRINT_LOG("END-OF-LOG")
  return 0;
}

Request::Request(void) {
  PRINT_FUNC();
  auth_redirect = false;
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
  file_content.clear();
  file_path.clear();
  file_type.clear();
  file_size = 0;
  read_size = 0;
  complete_file = false;
  in_response = false;
  read_count = 0;
  found_user = false;
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
