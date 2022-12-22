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
  Server serv;

  while (std::getline(stream, line)) {
    //std::cout << "line = " << line << std::endl;
    if (line == "{" || line == "}" || line == "\n")
      continue ;
    if (line == "server") {
      if (conf.size()) {
        conf.erase(std::remove(conf.begin(), conf.end(), '\t'), conf.end());
        serv.setup_server(conf);
        _serv.push_back(serv);
      //  std::cout << std::endl << "CONFIGURATION:" << std::endl << conf << std::endl << std::endl;
      }
      conf.clear();
      continue;
    }
    conf += line;
    conf += "\n";
  }
  if (conf.size()) {
    conf.erase(std::remove(conf.begin(), conf.end(), '\t'), conf.end());
    serv.setup_server(conf);
    _serv.push_back(serv);
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
