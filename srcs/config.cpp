#include "../includes/config.hpp"
#include <algorithm>
#include <iostream>

Config::Config(void) {
}

Config::~Config(void) {
}

void  Config::add_config(std::string &config) {

  std::cout << "In add config" << std::endl;
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
    serv.setup_server(vec);
    std::cout << "______________________ AFTER PUSH_BACK SERV __________________" << std::endl;
    _serv.push_back(serv);
    conf.clear();
    //std::cout << std::endl << "CONFIGURATION:" << std::endl << conf << std::endl << std::endl;
  }
}

std::ostream &operator<<(std::ostream &nstream, Config &config) {
  nstream << "CONFIG" << std::endl
    << "/////////////////////////////////" << std::endl;
  for (unsigned long i = 0; i < config._serv.size(); i++)
    nstream << config._serv[i];
  return nstream;
}
