#include "../includes/error.hpp"

//  Client side errors
std::string start = "<html><head><title>Webserv</title><link rel=\"icon\" type=\"image/x-icon\" href=\"/favicon.ico\"><style>.glass-panel {color: #fff;margin: 40px auto;background-color: rgba(255,255,255,0.3);border: 1px solid rgba(255,255,255,0.1);width: 100%;border-radius: 15px;padding: 32px;backdrop-filter: blur(10px);}.glass-button {display: inline-block;padding: 24px 32px;border: 0;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}html, body {margin: 0;height: 100%;}body {background: linear-gradient(45deg, rgb(62, 78, 102), rgb(192,177,184), rgb(194,152,163), rgb(170, 177, 189), rgb(135, 155, 177), rgb(107, 130, 162));background-size: 400% 400%;animation: gradient 10s ease infinite;}@keyframes gradient {  0% {    background-position: 0% 50%;  }  50% {    background-position: 100% 50%;  }  100% {    background-position: 0% 50%;  }}.glass-panel {  max-width: 600px;}.glass-button {margin: 200px;        margin-top: 40px;}h1, h1 a {  min-height: 120px;width: 90%;       max-width: 700px;       vertical-align: middle;       text-align: center;margin: 0 auto;        text-decoration: none;color: #fff;       padding-top: 60px;color: rgba(255,255,255,0.8);}p {width: 80%;margin: 0 auto;        padding-bottom: 32px;color: rgba(255,255,255,0.6);}</style></head><body><h1></h1><div class=\"glass-panel\"><h1><a>";

std::string end = "</a></h1><div class=\"glass-toolbar\"><a href=\"http://localhost:8080/html?disconnect=true\" target=\"_self\" class=\"glass-button\">Back to home</a></div></div></body></html>";


void  bad_request(int client) {
  std::string error;

  error = "HTTP/1.1 400 Bad Request\r\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "400 Bad Request";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  unauthorized(int client) {
  std::string error;

  error = "HTTP/1.1 401 Unauthorized\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "401 Unauthorized";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  forbidden(int client) {
  std::string error;

  error = "HTTP/1.1 403 Forbidden\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "403 Forbidden";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  not_found(int client) {
  std::string error;

  error = "HTTP/1.1 404 Not Found\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "404 Not Found";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  method_not_allowed(int client) {
  std::string error;

  error = "HTTP/1.1 405 Method Not Allowed\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "405 Method Not Allowed";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  gone(int client) {
  std::string error;

  error = "HTTP/1.1 410 Gone\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "410 Gone";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  length_required(int client) {
  std::string error;

  error = "HTTP/1.1 411 Length Required\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "411 Length Required";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  uri_too_long(int client) {
  std::string error;

  error = "HTTP/1.1 414 URI Too Long\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "414 URI Too Long";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  unsupported_media_type(int client) {
  std::string error;

  error = "HTTP/1.1 415 Unsupported Media Type\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "415 Unsupported Media Type";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

//  Server side errors
void  internal_server_error(int client) {
  std::string error;

  error = "HTTP/1.1 500 Internal Server Error\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "500 Internal Server Error";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  not_implemented(int client) {
  std::string error;

  error = "HTTP/1.1 501 Not Implemented\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "501 Not Implemented";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  http_version_not_supported(int client) {
  std::string error;

  error = "HTTP/1.1 505 HTTP Version Not Supported\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "505 HTTP Version Not Supported";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

void  insufficient_storage(int client) {
  std::string error;

  error = "HTTP/1.1 507 Insufficient Storage\n";
  error += "Content-Type: text/html\r\n\r\n";
  error += start;
  error += "507 Insufficient Storage";
  error += end;
  error += "\r\n";
  write(client, error.c_str(), error.size());
}

// Switch error

void  send_error(int client, int code) {
  switch (code) {
    case 400:
      bad_request(client);
      break;
    case 401:
      unauthorized(client);
      break;
    case 403:
      forbidden(client);
      break;
    case 404:
      not_found(client);
      break;
    case 405:
      method_not_allowed(client);
      break;
    case 410:
      gone(client);
      break;
    case 411:
      length_required(client);
      break;
    case 414:
      uri_too_long(client);
      break;
    case 415:
      unsupported_media_type(client);
      break;
    case 500:
      internal_server_error(client);
      break;
    case 501:
      not_implemented(client);
      break;
    case 505:
      http_version_not_supported(client);
      break;
    case 507:
      insufficient_storage(client);
      break;
  }
}
