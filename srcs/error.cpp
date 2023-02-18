#include "../includes/error.hpp"
#include "../includes/macro.hpp"
#include "../includes/string.hpp"


//  Client side errors
std::string start = "<html><head><title>Webserv</title><link rel=\"icon\" type=\"image/x-icon\" href=\"/favicon.ico\"><style>.glass-panel {color: #fff;margin: 40px auto;background-color: rgba(255,255,255,0.3);border: 1px solid rgba(255,255,255,0.1);width: 100%;border-radius: 15px;padding: 32px;backdrop-filter: blur(10px);}.glass-button {display: inline-block;padding: 24px 32px;border: 0;text-decoration: none;border-radius: 15px;background-color: rgba(255,255,255,0.1);border: 1px solid rgba(255,255,255,0.1);backdrop-filter: blur(30px);color: rgba(255,255,255,0.8);font-size: 14px;letter-spacing: 2px;cursor: pointer;text-transform: uppercase;transition: 1s;}.glass-button:hover {transform: scale(1.1);background-color: rgba(255,255,255,0.3);}html, body {margin: 0;height: 100%;}body {background: linear-gradient(45deg, rgb(62, 78, 102), rgb(192,177,184), rgb(194,152,163), rgb(170, 177, 189), rgb(135, 155, 177), rgb(107, 130, 162));background-size: 400% 400%;animation: gradient 10s ease infinite;}@keyframes gradient {  0% {    background-position: 0% 50%;  }  50% {    background-position: 100% 50%;  }  100% {    background-position: 0% 50%;  }}.glass-panel {  max-width: 600px;}.glass-button {margin: 200px;        margin-top: 40px;}h1, h1 a {  min-height: 120px;width: 90%;       max-width: 700px;       vertical-align: middle;       text-align: center;margin: 0 auto;        text-decoration: none;color: #fff;       padding-top: 60px;color: rgba(255,255,255,0.8);}p {width: 80%;margin: 0 auto;        padding-bottom: 32px;color: rgba(255,255,255,0.6);}</style></head><body><h1></h1><div class=\"glass-panel\"><h1><a>";

std::string end = "</a></h1><div class=\"glass-toolbar\"><a href=\"http://localhost:8080/cgi-bin/get.sh/html?disconnect=true\" target=\"_self\" class=\"glass-button\">Back to home</a></div></div></body></html>";

void  expect_continue(int client) {
  std::string error = "HTTP/1.1 100 Continue\n\n";

  write(client, error.c_str(), error.size());
}

void  bad_request(int client) {
  std::string error;
  std::string content = start + "400 Bad Request" + end + "\n\n";

  error = "HTTP/1.1 400 Bad Request\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;
  write(client, error.c_str(), error.size());
}

void  unauthorized(int client) {
  std::string error;
  std::string content = start + "401 Unauthorized" + end + "\n\n";

  error = "HTTP/1.1 401 Unauthorized\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;
  write(client, error.c_str(), error.size());
}

void  forbidden(int client) {
  std::string error;
  std::string content = start + "403 Forbidden" + end + "\n\n";

  error = "HTTP/1.1 403 Forbidden\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;
  write(client, error.c_str(), error.size());
}

void  not_found(int client) {
  std::string error;
  std::string content = start + "404 Not Found" + end + "\n\n";

  error = "HTTP/1.1 404 Not Found\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;
  write(client, error.c_str(), error.size());
}

void  method_not_allowed(int client) {
  std::string error;
  std::string content = start + "405 Method Not Allowed" + end + "\n\n";

  error = "HTTP/1.1 405 Method Not Allowed\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;
  write(client, error.c_str(), error.size());
}

void  gone(int client) {
  std::string error;
  std::string content = start + "410 Gone" + end + "\n\n";

  error = "HTTP/1.1 410 Gone\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;

  write(client, error.c_str(), error.size());
}

void  length_required(int client) {
  std::string error;
  std::string content = start + "411 Length Required" + end + "\n\n";

  error = "HTTP/1.1 411 Length Required\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;
  write(client, error.c_str(), error.size());
}

void  content_too_large(int client) {
  std::string error;
  std::string content = start + "413 Content Too Large" + end + "\n\n";

  error = "HTTP/1.1 413 Content Too Large\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;
  write(client, error.c_str(), error.size());
}

void  uri_too_long(int client) {
  std::string error;
  std::string content = start + "414 URI Too Long" + end + "\n\n";

  error = "HTTP/1.1 414 URI Too Long\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;
  write(client, error.c_str(), error.size());
}

void  unsupported_media_type(int client) {
  std::string error;
  std::string content = start + "415 Unsupported Media Type" + end + "\n\n";

  error = "HTTP/1.1 415 Unsupported Media Type\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;

  write(client, error.c_str(), error.size());
}

//  Server side errors
void  internal_server_error(int client) {
  std::string error;
  std::string content = start + "500 Internal Server Error" + end + "\n\n";

  error = "HTTP/1.1 500 Internal Server Error\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;

  write(client, error.c_str(), error.size());
}

void  not_implemented(int client) {
  std::string error;
  std::string content = start + "501 Not Implemented" + end + "\n\n";

  error = "HTTP/1.1 501 Not Implemented\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;

  write(client, error.c_str(), error.size());
}

void  http_version_not_supported(int client) {
  std::string error;
  std::string content = start + "505 HTTP Version Not Supported" + end + "\n\n";

  error = "HTTP/1.1 505 HTTP Version Not Supported\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;

  write(client, error.c_str(), error.size());
}

void  insufficient_storage(int client) {
  std::string error;
  std::string content = start + "507 Insufficient Storage" + end + "\n\n";

  error = "HTTP/1.1 507 Insufficient Storage\n";
  error += "Content-Type: text/html\n";
  error += "Content-Lenght: " + to_string(content.size()) + "\n\n";
  error += content;
 
  write(client, error.c_str(), error.size());
}

int  send_custom_error(int client, int code, std::string path) {
  std::string error;

  error = "HTTP/1.1 500 Internal Server Error\n";

  switch (code) {
    case 400:
      error = "HTTP/1.1 400 Bad Request\n";
      break;
    case 401:
      error = "HTTP/1.1 401 Unauthorized\n";
      break;
    case 403:
      error = "HTTP/1.1 403 Forbidden\n";
      break;
    case 404:
      error = "HTTP/1.1 404 Not Found\n";
      break;
    case 405:
      error = "HTTP/1.1 405 Method Not Allowed\n";
      break;
    case 410:
      error = "HTTP/1.1 410 Gone\n";
      break;
    case 411:
      error = "HTTP/1.1 411 Length Required\n";
      break;
    case 413:
      error = "HTTP/1.1 413 Content Too Large\n";
      break;
    case 414:
      error = "HTTP/1.1 414 URI Too Long\n";
      break;
    case 415:
      error = "HTTP/1.1 415 Unsupported Media Type\n";
      break;
    case 500:
      error = "HTTP/1.1 500 Internal Server Error\n";
      break;
    case 501:
      error = "HTTP/1.1 501 Not Implemented\n";
      break;
    case 505:
      error = "HTTP/1.1 505 HTTP Version Not Supported\n";
      break;
    case 507:
      error = "HTTP/1.1 507 Insufficient Storage\n";
      break;
  }
  error += "Content-type: text/html\n\n";
  
  std::ifstream errfile;
  std::string tmp;

  errfile.open(path.c_str());
  if (errfile.is_open()) {
    while (std::getline(errfile, tmp))
      error += tmp;
    error += "\n\n";
    write(client, error.c_str(), error.size());
  }
  else {
    return -1;
  }
  return 0;
}

void  send_error(int client, int code, Client &curr, std::string path) {
  struct stat st;

  if (path.size() && code != 100) {
    char  pwd[100];
    if (getcwd(pwd, 100) != NULL) {
      path = pwd + path;
      PRINT_LOG(path);
      if (stat(path.c_str(), &st) == 0) {
        if (S_ISREG(st.st_mode) != 0) {
          if (send_custom_error(client, code, path) == 0)
             return ;
        }
      }
    }
  }
  switch (code) {
    case 100:
      expect_continue(client);
      break;
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
    case 413:
      content_too_large(client);
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
    default:
      internal_server_error(client);
  }
  std::cout << curr << std::endl;
}

void  redirect_error(int client, int code) {

  std::string answer = "HTTP/1.1 307 Temporary Redirect\n";
              answer += "Location: http://localhost:8080/" + to_string(code) + "\r\n\r\n";

  write(client, answer.c_str(), answer.size());
}
