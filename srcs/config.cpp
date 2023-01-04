#include "../includes/config.hpp"
#include "../includes/macro.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>

Config::Config(void) {
}

Config::~Config(void) {
}

void  Config::add_config(std::string &config) {
  this->init_mime();
  PRINT_FUNC();
  std::istringstream stream(config);
  std::string line;
  std::string conf;
  std::vector<std::string> vec;
  Server serv;

  while (std::getline(stream, line)) {
    //std::cout << "config line = " << line << std::endl;
      //std::cout << "line[0] = " << static_cast<int>(line[0]) << std::endl;
    if (line == "{" || line == "}" || line == "\n" || !line[0]) {
      //std::cout << "line[0] = " << static_cast<int>(line[0]) << std::endl;
      continue ;
    }
    if (line == "server") {
      std::cout << "LINE EQUAL SERVER" << std::endl;
      if (conf.size()) {
        conf.erase(std::remove(conf.begin(), conf.end(), '\t'), conf.end());
        for (unsigned long int j = 0; j < vec.size(); j++) {
          vec[j].erase(std::remove(vec[j].begin(), vec[j].end(), '\t'), vec[j].end());
        }
        //std::cout << "CONF: " << std::endl << conf << std::endl << "XXXXXXXXXXXXX" << std::endl << std::endl;
        //serv.setup_server(conf);
        serv.setup_server(vec);
        std::cout << "______________________ PUSH_BACK SERV __________________" << std::endl;
        _serv.push_back(serv);
        //  std::cout << std::endl << "CONFIGURATION:" << std::endl << conf << std::endl << std::endl;
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
    //serv.setup_server(conf);
    std::cout << "_________BEFORE PUSHBACK SERV___________" << std::endl;
    serv.setup_server(vec);
    std::cout << "______________________ AFTER PUSH_BACK SERV __________________" << std::endl;
    _serv.push_back(serv);
    conf.clear();
    //std::cout << std::endl << "CONFIGURATION:" << std::endl << conf << std::endl << std::endl;
  }
}

void  Config::init_mime(void) {
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
    exit(0);
  }
 /* for (std::map<std::string, std::string>::iterator it = _mime.begin(); it != _mime.end(); it++) {
    std::cout << it->first << ":" << it->second << std::endl;
  }*/
}

std::ostream &operator<<(std::ostream &nstream, Config &config) {
  nstream << "CONFIG" << std::endl
    << "/////////////////////////////////" << std::endl;
  for (unsigned long i = 0; i < config._serv.size(); i++)
    nstream << config._serv[i];
  return nstream;
}
