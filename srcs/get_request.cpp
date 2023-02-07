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
  else if (serv[index]._login && (client._log == false || client._name.size() < 1) && comp(path_info, "user") == true) {
    auth_redirect = 2;
    tmp_path = "/login";
    //PRINT_ERR("Redirect to log");
  }
  else
    tmp_path = path_info;
  //PRINT_ERR("tmp_path: " + tmp_path);
  if (comp(tmp_path, "user") == true) {
    client._log = true;
  }
  //PRINT_LOG("path = " + path);
  PRINT_LOG("tmp path = " + tmp_path);
  if (flag != 0) {
    found = true;
    file_path = tmp_path;
    return ;
  }
  size_t x = 0;
  /*for (size_t i = 0; i < serv.size(); i++)
    if (comp(host, serv[i]._host) == true)
      x = i;*/
  x = index;
  //PRINT_LOG("Found host");
  if (tmp_path.compare("/") == 0)
  {
   PRINT_LOG("Using default server index");
   //std::cout << serv[x] << std::endl;
    file_path = serv[x]._index;
    PRINT_LOG("default index: " + file_path);
    while (file_path[0] == ' ' || file_path[0] == '\t')
      file_path.erase(0, 1);
    found = true;
  }
  else if (tmp_path.find(".ico") != std::string::npos)
  {
   //PRINT_LOG("Favicon Request");
    file_path = "./favicon.ico";
    found = true;
  }
  else
  {
    //PRINT_LOG("Custom index");
    //PRINT_LOG("Looking for path:" + path);
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
        file_path = serv[x]._loc[j]._root + tmp_path;
        DIR *dir = opendir(file_path.c_str());
        if (dir) {
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
  //PRINT_LOG("After series of if");
  if (found == false) {
    set_error(404);
    //PRINT_ERR("404 Not found");
  }
  if (flag != 0)
    return ;
  size_t extpos;
  std::string file_ext;

  PRINT_LOG(file_path);
  //PRINT_LOG("extension pos");
  if (file_path.size()) {
    extpos = file_path.find_last_of(".");
    if (extpos >= file_path.size()){
      content_type = ".txt";
      return ;
    }
    //PRINT_WIN(extpos);
    //PRINT_LOG("file ext");
    file_ext = file_path.substr(extpos);
    //PRINT_WIN(file_ext);
    //PRINT_LOG("content_type");
    content_type = file_ext;
    //PRINT_WIN(content_type);
  }
  //PRINT_LOG("End of function");
}

void  Request::get_request(std::vector<Server> &serv, Client &client, int index) {
  std::ifstream file;
  int bsize = buff_size * 2;
  char buff[bsize];
  std::string ascii;

  if (file_path.size() < 1)
    this->get_file(serv, client, path_info, file_path, index);
  
  if (auth_redirect == 3) {
    complete_file = true;
    return ;
  }
  if (complete_file == true) {
    PRINT_WIN("File complete");
    return ;
  }
  if (file_size > 0) {
    read_count++;
    ascii = std::to_string(read_count);
    //PRINT_LOG(ascii + " time opening");
    //PRINT_LOG("Read size: " + to_string(read_size));
    //PRINT_LOG("File size: " + to_string(file_size));
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
      file_content.pop_back();
    ascii = std::to_string(file_size - read_size);
    //PRINT_LOG("Filesize - readsize: " + ascii);
    size_t curr_size = file_content.size();
    for (size_t i = 0; i < last_read; i++)
      file_content += buff[i];
    /*if (found_user == false && comp(content_type, "html") == true) {
      found_user = replace(file_content, "$@", client._name, curr_size);
    }*/
    (void)curr_size;
    file.close();
  }
  else {
    //PRINT_LOG("First file opening");
    //PRINT_LOG(file_path);
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
        file_content.pop_back();
      ascii = std::to_string(file_size);
      //PRINT_LOG("Filesize: " + ascii);
      ascii = std::to_string(file_size - read_size);
      //PRINT_LOG("Filesize - readsize: " + ascii);
      file_content.clear();
      for (size_t i = 0; i < read_size; i++)
        file_content += buff[i];
      //file_content = buff; 
      /*if (found_user == false && comp(content_type, "html") == true) {
        found_user = replace(file_content, "$@", client._name);
        //PRINT_LOG("Replaced: " + to_string(found_user));
      }*/
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
  if (comp(path, "download") == false) {
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
    //PRINT_WIN(file_path);
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
    // answer += "Connection: keep-alive\n";.

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
      answer += file_content;
      answer += "\r\n\r\n";
  }
}
