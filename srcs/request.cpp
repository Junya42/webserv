#include "../includes/request.hpp"
#include <sstream>
#include <unistd.h>

void  Request::get_header(std::string &request) {
  std::istringstream stream(request);
  std::string line;

  stream >> method >> path >> version;
  while (std::getline(stream, line)) {
    if (line == "\r\n\r\n") {
      complete_header = true;
      break ;
    }
    std::istringstream line_stream(line);
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);

    header[key] = value;
    if (key.find("Content-Length") != std::string::npos) {
      content_lenght = atoi(value.c_str());
      has_body = true;
    }
    else if (key.find("Host") != std::string::npos) {
      host = value;
    }
    else if (key.find("Content-Type: multipart/form-data") != std::string::npos) {
      std::istringstream bound(value);
      std::getline(bound, key, '=');
      std::getline(bound, boundary);
    }
  }
}

void  Request::get_body(int client) {
  if (content_lenght > 0 && (current_bytes = read(client, buffer, sizeof(buffer))) > 0) {
    body_str += buffer;
    content_lenght -= current_bytes;
  }
}

std::string check_method(std::string &method) {
  if (method == "GET" || method == "POST" || method == "DELETE")
    return std::string("", 0);
  if (method == "HEAD" || method == "PUT" || method == "CONNECT" || method == "OPTIONS" || method == "TRACE" || method == "PATCH")
    return std::string("501 Not Implemented");
  return std::string ("400 Bad Request");
}

int Request::parse_header(void) {
  std::string method_buffer;

  method_buffer = check_method(method);
  if (method.size() > 1) {
    answer += method_buffer;
    return -1;
  }
  if (method == "POST" && content_lenght < 1)
    return -1;
  if (!host.size())
    return -1;
  parsed_header = true;
  return 0;
}

int  Request::parse_body(void) {
  if (boundary.size()) {
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
      if (boundary_count % 2)
        return -1;
    }
    else
      return -1;
  }
  else {
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
  return 0;
}

void  Request::clear(void) {
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

  if (parsed_body == true && parsed_header == true)
    return 1;
  if (complete_header == false) {
    if ((current_bytes = read(client, buffer, sizeof(buffer))) > 0) {
      bytes += current_bytes;
      request += buffer;
    }
    if (current_bytes < 0)
      return -1;
    get_header(request);
  }
  if (complete_header == true && has_body && content_lenght > 0)
    get_body(client);
  if (complete_header && parsed_header == false)
    parse_header();
  if (complete_header && has_body && content_lenght < 1 && parsed_body == false)
    parse_body();
  if (complete_header && has_body == false)
    parsed_body = true;
  if (parsed_body == true && parsed_header == true)
    return 1;
  return 0;
}

Request::Request(void) {
  answer = "HTTP/1.1 ";
  header_code = 0;
  has_body = false;
  complete_header = false;
  parsed_body = false;
  parsed_header = false;
  bytes = 0;
  current_bytes = 0;
}

Request::~Request(void) {
}

Request::Request(std::string &request) {
  (void)request;
}
