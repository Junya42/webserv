#include "../includes/client.hpp"
#include <cstdlib>
#include <sstream>
#include <string>
#include <unistd.h>

void  Request::get_file(std::vector<Server> &serv, Client &client, std::string &path_info, std::string &file_path, int index, size_t flag) {

  PRINT_ERR("Path: " + path_info);
  PRINT_ERR("Log: " + to_string(client._log));
  PRINT_ERR("Username: " + client._name);
  std::string tmp_path;
  bool found = false;
  if (using_cgi == false && comp(path_info, "?disconnect") == true) {
    for (size_t i = 0; i < path_info.size(); i++)
      if (path_info[i] == '?')
        tmp_path = path_info.substr(0, i);
  }
  else if (serv[index]._redirect && name.size() && auth == true && (path_info== "/" || path_info == "/html" || comp(path, "/login") == true)) {
    auth_redirect = 1;
    tmp_path = "/user";
    PRINT_WIN("Auth redirect");
  }
  else if (serv[index]._login && serv[index]._redirect && (client._log == false || client._name.size() < 1) && comp(path_info, "user") == true) {
    auth_redirect = 2;
    tmp_path = "/login";
    PRINT_ERR("Redirect to log");
  }
  else if (serv[index]._login && (client._log == false || client._name.size() < 1) && comp(path_info, "user") == true) {
    set_error(401);
  }
  else
    tmp_path = path_info;
  if (comp(tmp_path, "user") == true) {
    client._log = true;
  }
  PRINT_LOG("tmp path = " + tmp_path);
  if (flag != 0) {
    found = true;
    file_path = tmp_path;
    return ;
  }
  size_t x = 0;
  x = index;
  if (tmp_path.compare("/") == 0 && serv[x]._index.size())
  {
   PRINT_LOG("Using default server index");
    file_path = serv[x]._index;
    PRINT_LOG("default index: " + file_path);
    while (file_path[0] == ' ' || file_path[0] == '\t')
      file_path.erase(0, 1);
    found = true;
  }
  else if (tmp_path.find(".ico") != std::string::npos)
  {
    file_path = "./favicon.ico";
    found = true;
  }
  else
  {
    for (size_t j = 0; j < serv[x]._loc.size(); j++) {
      if (comp(tmp_path, serv[x]._loc[j]._path) == true) {
        if (serv[x]._loc[j]._redirect == true) {
          link = serv[x]._loc[j]._link;
          PRINT_WIN("Redirect to " + link);
          auth_redirect = 3;
          using_cgi = false;
          return ;
        }
        if (flag == 0 && serv[x]._loc[j].method[method] != true) {
          PRINT_ERR("SET ERROR 405");
          for (std::map<std::string, bool>::iterator it = serv[x]._loc[j].method.begin(); it != serv[x]._loc[j].method.end(); it++)
            std::cout << std::boolalpha << it->first << " : " << it->second << std::endl;
          set_error(405);
          complete_file = true;
          return ;
        }
        if (flag == 0 && serv[x]._loc[j]._mbsize >= 0 && initial_lenght > serv[x]._loc[j]._mbsize) {
          PRINT_ERR("SET ERROR 400");
          set_error(400);
          complete_file = true;
          return ;
        }
        file_path = serv[x]._loc[j]._root + tmp_path;
        DIR *dir = opendir(file_path.c_str());
        PRINT_LOG("file = " + file_path);
         if (dir) {
          PRINT_LOG("DIR");
          PRINT_LOG(serv[x]._loc[j]._path);
          if (serv[x]._loc[j]._index.size())
            file_path = serv[x]._loc[j]._index;
          else if (serv[x]._loc[j]._autoindex == true && using_cgi == false) {
              auto_file_name(client);
              auto_index = true;
              complete_file = true;
              closedir(dir);
              return ;
          }
          else
            file_path += "/index.html";
          closedir(dir);
        }
        struct stat st;
        if (stat(file_path.c_str(), &st) == 0) {
          found = true;
          PRINT_WIN("Found filename");
          PRINT_WIN(file_path);
        }
        break ;
      }
    }
  }
  if (found == false) {
    set_error(404);
  }
  if (flag != 0)
    return ;
  size_t extpos;
  std::string file_ext;

  PRINT_LOG(file_path);
  if (file_path.size()) {
    extpos = file_path.find_last_of(".");
    if (extpos >= file_path.size()){
      content_type = ".txt";
      return ;
    }
    file_ext = file_path.substr(extpos);
    content_type = file_ext;
  }
}

void  Request::get_request(std::vector<Server> &serv, Client &client, int index) {
  std::ifstream file;
  int bsize = buff_size * 2;
  char buff[bsize];
  std::string ascii;

  if (file_path.size() < 1)
    this->get_file(serv, client, path_info, file_path, index);
  
  if (auth_redirect == 3 || complete_file == true || header_code != 0) {
    complete_file = true;
    return ;
  }
 
  if (file_size > 0) {
    read_count++;
    ascii = to_string(read_count);
    file.open(file_path.c_str(), std::ios::in | std::ios::binary);
    file.seekg(read_size, file.beg);
    file.read(buff, bsize);
    size_t last_read = file.gcount();
    read_size += last_read;
    if (file_size - read_size <= 0) {
      PRINT_WIN("File complete in : " + to_string(read_count) + " runs");
      complete_file = true;
    }
    else
      file_content.erase(file_content.size() - 1, 1);
    ascii = to_string(file_size - read_size);
    size_t curr_size = file_content.size();
    for (size_t i = 0; i < last_read; i++)
      file_content += buff[i];
    (void)curr_size;
    file.close();
  }
  else {
    file.open(file_path.c_str(), std::ios::in | std::ios::binary);
    complete_file = false;
    if (file.is_open()) {

      file.seekg(0, file.end);
      read_size = file.tellg();
      file_size = read_size;
      file.seekg(0, file.beg);

      file.read(buff, buff_size);
      read_size = file.gcount();
      if (file_size - read_size == 0) {
        PRINT_WIN("File complete");
        complete_file = true;
      }
      else
        file_content.erase(file_content.size() - 1, 1);
      ascii = to_string(file_size - read_size);
      file_content.clear();
      for (size_t i = 0; i < read_size; i++) {
        file_content.push_back(buff[i]);
      }
      file.close();
    }
    else {
      set_error(404);
      PRINT_ERR("404 Not Found");
      status = "HTTP/1.1 404 Not Found\n";
      file_content = "<html><head><title>Error 404</title></head><body><h1>ERROR 404</h1></body></html>";
      file_path = "error.html";
      read_size = file_content.size();
      complete_file = true;
    }
  }
}

void  Request::set_content_type(std::map<std::string, std::string> &_mime, size_t flag) {
  size_t extpos;
  std::string file_ext;

  extpos = file_path.find_last_of(".");
  content_type.clear();
  if (extpos >= file_path.size()) {
    if (flag == 0)
      content_type = "Content-Type: text/plain\n";
    else {
      content_type = "text/plain";
    }
    return;
  }
  if (flag == 0)
    content_type = "Content-Type: ";
  file_ext = file_path.substr(extpos);
  content_type += get_mime(file_ext, _mime);
  if (flag == 0)
    content_type += "\n";
}

void  Request::get_response(std::map<std::string, std::string> &_mime, Client &client) {
  std::ostringstream s;
  bool redirect = false;
  PRINT_WIN(path);
  if (comp(path, "download") == false && auto_index == false) {
    if (comp(path, "?disconnect=true") == true) {
      redirect = true;
      client._log = false;
      status = "HTTP/1.1 307 Temporary Redirect\nLocation: http://" + client._host + ":" + client._sport + "/html\n";
    }
    else if (auth_redirect == 1 && auth == true && client._cookie.size()) {
      status = "HTTP/1.1 307 Temporary Redirect\nLocation: http://" + client._host + ":" + client._sport + "/user\n";
    }
    else if (auth_redirect == 2) {
      status = "HTTP/1.1 307 Temporary Redirect\nLocation: http://" + client._host + ":" + client._sport + "/login\n";
    }
    else if (auth_redirect == 3) {
      if (method == "GET")
        answer = "HTTP/1.1 307 Moved Temporary Redirect\n";
      else
        answer = "HTTP/1.1 303 See Other\n";
      answer += "Location: " + link + "\n\n";
      return ;
    }
    if (comp(method, "delete") == false)
      this->set_content_type(_mime);
    else {
      answer = "HTTP/1.1 200 OK\n";
      answer += "Content-Type: text/html\n\n";
      answer += file_content;
      answer += "\n\n";
      return ;
    }

    s << read_size;
    answer = status;

    PRINT_LOG(file_path);
    if (client._name.size() && comp(file_path, "/user/index.html") == true && client._fav == false) {
      PRINT_WIN("Adding cookie to: " + client._name);
      client._log = true;
      client._cookie = "log=" + client._name;
      answer += "Set-Cookie: ";
      answer += client._cookie;
      answer += "; Path=/; Expires= Fri, 5 Oct 2042 14:42:00 CMT;\n";
    }
    else if (redirect == true) {
      PRINT_ERR("Remove cookie from: " + client._name);
      std::string tmp = "log=" + client._name + "; Path=/; Expires=Fri, 5 Oct 2018 14:42:00 GMT;\n";
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
  else {
      answer.clear();
      answer = "HTTP/1.1 200 OK\n";
      answer += "Content-Lenght: " + to_string(file_content.size());
      answer += "\n\n";
      answer += file_content;
      answer += "\r\n\r\n";
  }
}
