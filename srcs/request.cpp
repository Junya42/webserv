#include "../includes/request.hpp"
#include <sstream>
#include <unistd.h>

void  Request::get_header(std::string &request) {
  std::istringstream stream(request);
  std::string line;
  size_t  line_count = 0;

  PRINT_FUNC();
  std::cout << "_____________________________" << std::endl
    << "\033[1;32mRequest: \033[0m" << std::endl
    << request << std::endl;

  if (method.size() < 1)
    stream >> method >> path >> version;
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
    PRINT_LOG(std::string(key + " : " + value, 0, key.size() + value.size() + 2));
    if (comp(key, "Content-Length") == true) {
      content_lenght = atoi(value.c_str());
      body_size = content_lenght;
      has_size = true;
      has_body = true;
    }
    else if (comp(key, "Content-type") == true) {
      content_lenght = 1;
      has_body = true;
    }
    else if (comp(key, "Host") == true) {
      host = value;
    }
    else if (comp(key, "Content-Type: multipart/form-data") == true) {
      std::istringstream bound(value);
      std::getline(bound, key, '=');
      std::getline(bound, boundary);
    }
    line_count++;
  }
  std::cout << "_____________________________" << std::endl;
}

void  Request::get_body(int client) {
  PRINT_FUNC();

  int int_bytes = 0;
  if (content_lenght > 0 && has_size == true && (int_bytes = read(client, buffer, sizeof(buffer))) > 0) {
    current_bytes = int_bytes;
    PRINT_LOG("Reading body with content_lenght");
    body_str += buffer;
    content_lenght -= current_bytes;
  }
  else if (has_body == true && has_size == false) {
    int_bytes = read(client, buffer, sizeof(buffer));
    PRINT_LOG("Reading body without content_lenght");
    current_bytes = int_bytes;
    std::cout << "sizeof buffer: " << sizeof(buffer) << std::endl;
    std::cout << "current_bytes: " << int_bytes << std::endl;
    if (int_bytes < 1 || current_bytes < sizeof(buffer)) {
      content_lenght = 0;
    }
    body_str += buffer;
  }
}

std::string check_method(std::string &method) {
  PRINT_FUNC();
  if (method == "GET" || method == "POST" || method == "DELETE") {
    PRINT_WIN("Success");
    return std::string("", 0);
  }
  if (method == "HEAD" || method == "PUT" || method == "CONNECT" || method == "OPTIONS" || method == "TRACE" || method == "PATCH") {
    PRINT_ERR("501 Not Implemented");
    return std::string("501 Not Implemented");
  }
  PRINT_ERR("400 Bad Request");
  return std::string ("400 Bad Request");
}

int Request::parse_header(void) {
  PRINT_FUNC();
  std::string method_buffer;

  method_buffer = check_method(method);
  if (method_buffer.size() > 1) {
    PRINT_ERR(method_buffer);
    answer += method_buffer;
    return -1;
  }
  if (method == "POST" && has_body == false) {
    PRINT_ERR("Couldn't find body in POST request");
    return -1;
  }
  if (!host.size()) {
    PRINT_ERR("Couldn't find host");
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
      std::string cpy(body_str, pos);
      std::istringstream bodystream(cpy);
      body_str.clear();
      int boundary_count = 0;
      while (std::getline(bodystream, cpy)) {
        if (cpy.find("Content-Disposition") != std::string::npos)
          tmpbody.disposition = cpy;
        else if (cpy.find("Content-Type") != std::string::npos)
          tmpbody.type = cpy;
        if (cpy.find(boundary) != std::string::npos)
          boundary_count++;
        else
          tmpbody.data.push_back(cpy);
        if (boundary_count % 2 == 0) {
          multi_body.push_back(tmpbody);
          tmpbody.clear();
        }
      }
      if (boundary_count % 2) {
        PRINT_ERR("Uneven Boundary count");
        return -1;
      }
    }
    else {
      PRINT_ERR("Couldn't find Boundary in Body str");
      return -1;
    }
  }
  else if (0) { //DANGER DANGER //DANGER DANGER //DANGER DANGER //DANGER DANGER //DANGER DANGER //DANGER DANGER
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
}

int  Request::read_client(int client) {
  std::string request;
  int int_bytes = 0;

  PRINT_FUNC();
  if (parsed_body == true && parsed_header == true) {
    PRINT_WIN("Success");
    return 1;
  }
  if (complete_header == false) {
    if ((int_bytes = read(client, buffer, sizeof(buffer))) > 0) {
      current_bytes = int_bytes;
      bytes += current_bytes;
      request += buffer;
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
    get_header(request);
  }
  if (complete_header == true && has_body)
    get_body(client);
  if (complete_header && parsed_header == false)
    parse_header();
  if (complete_header && has_body && content_lenght < 1 && parsed_body == false)
    parse_body();
  if (complete_header && has_body == false)
    parsed_body = true;
  if (parsed_body == true && parsed_header == true) {
    PRINT_WIN("Success");
    return 1;
  }
  PRINT_LOG("Uncomplete request or bad request");
  std::cout << *this << std::endl;
  PRINT_LOG("END-OF-LOG")
    return 0;
}

Request::Request(void) {
  PRINT_FUNC();
  has_size = false;
  in_use = false;
  answer = "HTTP/1.1 ";
  header_code = 0;
  has_body = false;
  complete_header = false;
  parsed_body = false;
  parsed_header = false;
  bytes = 0;
  current_bytes = 0;
  body_size = 0;
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
  for (std::vector<Body>::iterator it = req.multi_body.begin(); it != req.multi_body.end(); it++) {
    n << "  disposition:" << it->disposition << std::endl
      << "  type:" << it->type << std::endl
      << "  data:" << std::endl;
    for (std::vector<std::string>::iterator sit = it->data.begin(); sit != it->data.end(); sit++) {
      n << *sit << std::endl;
    }
    n << std::endl;
  }
  n << "\033[1m\033[2mBody str:\033[0m" << req.body_str << std::endl;
  n << std::endl << "\033[1m\033[2mAnswer:\033[0m" << req.answer << std::endl << std::endl;
  return n;
}
