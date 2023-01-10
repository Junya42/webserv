#include "../includes/client.hpp"
#include <cstdlib>
#include <sstream>
#include <string>
#include <unistd.h>

void  Request::get_file(std::vector<Server> &serv) {
  PRINT_FUNC();
  std::string tmp_path;
  if (comp(path, "?disconnect") == true) {
    for (size_t i = 0; i < path.size(); i++)
      if (path[i] == '?')
        tmp_path = path.substr(0, i);
  }
  else
    tmp_path = path;
  //PRINT_LOG("path = " + path);
  //PRINT_LOG("tmp path = " + tmp_path);
  for (size_t i = 0; i < serv.size(); i++) {
    if (comp(host, serv[i]._host) == true) {
   //PRINT_LOG("Found host");
      if (tmp_path.compare("/") == 0)
      {
      //PRINT_LOG("Using default server index");
        file_path = serv[i]._index;
        while (file_path[0] == ' ' || file_path[0] == '\t')
          file_path.erase(0, 1);
      }
      else if (tmp_path.find(".ico") != std::string::npos)
      {
     //PRINT_LOG("Favicon Request");
        file_path = "./favicon.ico";
      }
      else
      {
     //PRINT_LOG("Custom index");
       //PRINT_LOG("Looking for path:" + path);
        for (size_t j = 0; j < serv[i]._loc.size(); j++) {
       //PRINT_LOG(serv[i]._loc[j]._path);
          if (comp(tmp_path, serv[i]._loc[j]._path) == true) {
            file_path = serv[i]._loc[j]._root + tmp_path + "/" + "index.html";
        //PRINT_WIN("Found filename");
            break ;
          }
        }
        break ;
      }
    }
  }
  size_t extpos;
  std::string file_ext;

  extpos = file_path.find_last_of(".");
  file_ext = file_path.substr(extpos);
  content_type = file_ext;
}

void  Request::get_request(std::vector<Server> &serv, Client &client) {
  PRINT_FUNC();
  std::ifstream file;
  char buff[buff_size];
  std::string ascii;

  if (file_path.size() < 1)
    this->get_file(serv);
  if (complete_file == true) {
   PRINT_WIN("File complete");
    return ;
  }
  if (file_size > 0) {
    read_count++;
    ascii = std::to_string(read_count);
  //PRINT_LOG(ascii + " time opening");
    file.open(file_path.c_str(), std::ifstream::in | std::ifstream::binary);
    file.seekg(read_size, file.beg);
    file.read(buff, sizeof(buff));
    read_size += file.gcount();
    if (file_size - read_size == 0) {
   //PRINT_WIN("File complete in n runs");
      complete_file = true;
    }
    ascii = std::to_string(file_size - read_size);
  //PRINT_LOG("Filesize - readsize: " + ascii);
    size_t curr_size = file_content.size();
    file_content += buff;
    if (found_user == false && comp(content_type, "html") == true)
      found_user = replace(file_content, "$@", client._name, curr_size);
    file.close();
  }
  else {
   //PRINT_LOG("First file opening");
   //PRINT_LOG(file_path);
    file.open(file_path.c_str(), std::ifstream::in | std::ifstream::binary);
    complete_file = false;
    if (file.is_open()) {

      file.seekg(0, file.end);
      read_size = file.tellg();
      file_size = read_size;
      file.seekg(0, file.beg);

      file.read(buff, sizeof(buff));
      read_size = file.gcount();
      if (file_size - read_size == 0) {
      PRINT_WIN("File complete");
        complete_file = true;
      }
      ascii = std::to_string(file_size);
  //PRINT_LOG("Filesize: " + ascii);
      ascii = std::to_string(file_size - read_size);
    //PRINT_LOG("Filesize - readsize: " + ascii);
      file_content = buff; 
      if (found_user == false && comp(content_type, "html") == true)
        found_user = replace(file_content, "$@", client._name);
      file.close();
    }
    else {
   PRINT_ERR("404 Not Found");
      status = "HTTP/1.1 404 Not Found\n";
      file_content = "<html><head><title>Error 404</title></head><body><h1>ERROR 404</h1></body></html>";
      file_path = "error.html";
      read_size = file_content.size();
      complete_file = true;
    }
  }
}

void  Request::set_content_type(std::map<std::string, std::string> &_mime) {
  size_t extpos;
  std::string file_ext;

  extpos = file_path.find_last_of(".");
  content_type = "Content-Type: ";
  file_ext = file_path.substr(extpos);
  content_type += get_mime(file_ext, _mime);
  content_type += "\n";
}

void  Request::get_response(std::map<std::string, std::string> &_mime, Client &client) {
  std::ostringstream s;
  bool redirect = false;
  //PRINT_WIN(path);
  if (comp(path, "?disconnect=true") == true) {
    redirect = true;
    client._log = false;
    status = "HTTP/1.1 307 Temporary Redirect\nLocation: http://localhost:8080/html\n";
  }
  //PRINT_WIN(file_path);
  this->set_content_type(_mime);
  s << read_size;
  answer = status;
 // answer += "Connection: keep-alive\n";
  if (client._name.size() && comp(file_path, "/user/index.html") == true && client._fav == false) {
    PRINT_WIN("Adding cookie to: " + client._name);
    client._log = true;
    client._cookie = "log=" + client._name + "\n";
    answer += "Set-Cookie: ";
    answer += client._cookie;
  }
  else if (redirect == true) {
    PRINT_ERR("Remove cookie from: " + client._name);
    std::string tmp = "log=" + client._name + "; Expires=Fri, 5 Oct 2018 14:42:00 GMT;\n";
    answer += "Set-Cookie: ";
    answer += tmp;
  }
  answer += content_type;
  if (comp(content_type, "html") == false)
    answer += "Content-Transfer-Encoding: binary\n";
  answer += "Content-Length: ";
  answer += s.str();
  answer += "\r\n\r\n";
  answer += file_content;
  answer += "\r\n\r\n";
}
