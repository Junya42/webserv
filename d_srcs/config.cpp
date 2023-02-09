#include "../includes/config.hpp"
#include "../includes/macro.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

Config::Config(void) {
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  _pwd = cwd;
  _pwd += "/";
  connection_count = 0;
  request_count = 0;
  _serv.clear();
  _mime.clear();
}

Config::~Config(void) {
}

int  Config::add_config(std::string &config) {
  if (this->init_mime() == -1)
    return -1;
  PRINT_FUNC();
  //std::cout << std::endl << "---------------" << std::endl;
  std::istringstream stream(config);
  std::string line;
  std::string conf;
  std::vector<std::string> vec;
  Server serv;

  while (std::getline(stream, line)) {
    if (line == "{" || line == "}" || line == "\n" || !line[0]) {
      continue ;
    }
    if (line == "server") {
      if (conf.size()) {
        conf.erase(std::remove(conf.begin(), conf.end(), '\t'), conf.end());
        for (unsigned long int j = 0; j < vec.size(); j++) {
          vec[j].erase(std::remove(vec[j].begin(), vec[j].end(), '\t'), vec[j].end());
        }
        if (serv.setup_server(vec) == -1)
          return -1;
        _serv.push_back(serv);
        serv.clear();
      }
      vec.clear();
      conf.clear();
      continue;
    }
    vec.push_back(line);
    conf += line;
    conf += "\n";
  }
  if (conf.size()) {
    conf.erase(std::remove(conf.begin(), conf.end(), '\t'), conf.end());
    for (unsigned long int j = 0; j < vec.size(); j++)
      vec[j].erase(std::remove(vec[j].begin(), vec[j].end(), '\t'), vec[j].end());
    if (serv.setup_server(vec) == -1)
      return -1;
    _serv.push_back(serv);
    conf.clear();
  }
  for (std::vector<Server>::iterator it = _serv.begin(); it != _serv.end(); it++) {
    if (it->_ip.compare("error") == 0)
    {
      PRINT_ERR("ip = error for : " + it->_name);
      it->_valid = false;
    }
    for (std::vector<Server>::iterator itn = it + 1; itn < _serv.end(); itn++) {
      if (it->_ip.compare(itn->_ip) == 0 && it->_sport.compare(itn->_sport) == 0) {
        PRINT_ERR("Server:\033[1;31m" + it->_name + "\033[0m and server:\033[1;31m" + itn->_name + "\033[0m have the same ip and port");
        PRINT_ERR("Removing" + it->_name);
        it->_valid = false;
      }
    }
  }
  size_t i = 0;
  while (i < _serv.size()) {
    if (_serv[i]._valid == false) {
      _serv.erase(_serv.begin() + i);
      continue ;
    }
    else {
      std::string tmpname = _serv[i]._name;
      erase(tmpname, " ");
      std::string tmp = "/tmp/private_webserv/" + tmpname;
      struct stat st;

      if (stat(tmp.c_str(), &st) == -1)
        if (mkdir(tmp.c_str(), 0777) == -1) {
          PRINT_ERR("Couldn't create server directory for : " + _serv[i]._host);
          return -1;
        }
    }
    i++;
  }
  if (_serv.empty() == true) {
    PRINT_ERR("No functionnal server found");
    return -1;
  }
  return 0;
}

int  Config::init_mime(void) {
  PRINT_FUNC();
  _mime.clear();
  std::ifstream mime_reader;
  std::string line;
  std::string key;
  std::string value;

  mime_reader.open("./configs/mime_type");

  if (mime_reader.is_open()) {
    while (std::getline(mime_reader, line)) {
      if (line[0] == '#')
        continue ;
      std::istringstream  line_stream(line);
      std::getline(line_stream, key, '\t');
      std::getline(line_stream, value);

      while (value[0] == '\t' || value[0] == ' ')
        value.erase(0, 1);
      _mime[key] = value;
    }
  }
  else {
    PRINT_ERR("Couldn't get mime type");
    PRINT_ERR("Can't handle requests");
    PRINT_ERR("Please make sure your mime_type file located at configs/ is readable");
    PRINT_ERR("Exiting server");
    return -1;
  }
  return 0;
 /* for (std::map<std::string, std::string>::iterator it = _mime.begin(); it != _mime.end(); it++) {
    std::cout << it->first << ":" << it->second << std::endl;
  }*/
}

std::ostream &operator<<(std::ostream &nstream, Config &config) {
  for (size_t i = 0; i < config._serv.size(); i++) {
    nstream << std::endl << "________________________________________________________" << std::endl << std::endl
    << "\033[48;5;237m\033[38;5;81mServer " << i + 1 << "\033[0m" << std::endl << std::endl << config._serv[i];
    nstream << std::endl << "________________________________________________________" << std::endl
    << "********************************************************";
  }
  nstream << std::endl;
  return nstream;
}
