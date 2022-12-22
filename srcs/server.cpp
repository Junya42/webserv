#include "../includes/server.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <sys/types.h>

Server::Server(void) {
}

void  Server::setup_server(std::string & config) {
  std::istringstream stream(config);
  std::string location_conf;
  std::string line;
  Location loc;
  bool  location = false;

  while (std::getline(stream, line)) {
    std::istringstream line_stream(line);
    std::string key;
    std::string value;

    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);

    if (value.size())
      value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
    if (key == "location") {
      if (location == true) {
        loc.create_map(location_conf);
        _loc.push_back(loc);
       // std::cout << std::endl << "LOCATION CONF" << std::endl << location_conf << "END OF LOCATION CONF" <<std::endl;
      }
      location = true;
      location_conf.clear();
    }
    if (location) {
      location_conf += line;
      location_conf += '\n';
    }
    else {
      //std::cout << line << std::endl;
      if (key == "listen")
        _port = atoi(value.c_str());
      else if (key == "server_name")
        _name = value;
      else if (key == "index")
        _index = value;
      else if (key == "method_accept")
        _methods = value;
      //else
        //std::cout << "Unknown key: " << key << " | value: " << value << std::endl;
    }
    //std::cout << "key:" << key << std::endl << "value:" << value << std::endl << std::endl;
  }
}

Server::Server(std::string &config) {
  std::cout << "config: " << config << std::endl << std::endl;
  std::istringstream stream(config);
  std::string location_conf;
  std::string line;
  bool  location = false;

  while (std::getline(stream, line)) {
    std::istringstream line_stream(line);
    std::string key;
    std::string value;
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);
    if (key == "location") {
      if (location == true) {
        _loc.push_back(Location(location_conf));
        //std::cout << std::endl << location_conf << std::endl;
      }
      location = true;
      location_conf.clear();
    }
    if (location)
      location_conf += line;
    else {
      //std::cout << line << std::endl;
      if (key == "listen")
        _port = atoi(value.c_str());
      else if (key == "server_name")
        _name = value;
      else if (key == "index")
        _index = value;
      else if (key == "method_accept")
        _methods = value;
      //else
        //std::cout << "Unknown key: " << key << " | value: " << value << std::endl;
    }
  }
}

Server::~Server(void) {
}

std::string &Server::get_name(void) {
  return _name;
}

std::string &Server::get_index(void) {
  return _index;
}

ssize_t Server::get_size(void) {
  return _bodysize;
}

int Server::get_port(void) {
  return _port;
}

bool  Server::method_get(void) {
  return _get;
}

bool  Server::method_post(void) {
  return _post;
}

bool  Server::method_delete(void) {
  return _delete;
}

std::ostream &operator<<(std::ostream &nstream, Server &server) {
  nstream << "Server" << std::endl
    << "name = " << server._name << std::endl
    << "index = " << server._index << std::endl
    << "methods = " << server._methods << std::endl
    << "port = " << server._port << std::endl 
    << "_____________________________________" << std::endl << std::endl;

  int i = 1;
  for (unsigned long j = 0; j < server._loc.size(); j++) {
    nstream << "Location " << i++ << std::endl;
    nstream << server._loc[j] << std::endl
      << "---------------" << std::endl;
  }
  return nstream;
}
