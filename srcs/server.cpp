#include "../includes/server.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <sys/types.h>

const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

Server::Server(void) {
  _port = -1;
}


void  Server::setup_server(std::vector<std::string> &vec) {
//  std::cout << config << std::endl << "______________________" << std::endl;
  std::string location_conf;
  std::string line;
  Location loc;
  bool  location = false;

  std::cout << "Testing vector for server" << std::endl;
  for (long unsigned int j = 0; j < vec.size(); j++) {
    std::cout << vec[j] << std::endl;
  }
  std::cout << "end of test" << std::endl;
  std::cout << "WHILE" << std::endl;
  long unsigned int i = 0;
  while (i < vec.size()) {
    line = vec[i];
  //  std::cout << "LINE: " << line << std::endl;
    std::istringstream line_stream(line);
    std::string key;
    std::string value;

    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);

    if (value.size())
      trim(value);
    //value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
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
     // std::cout << "key: " << key << std::endl;
      if (key == "listen") {
        _port = atoi(value.c_str());
        _sport = value;
      }
      if (key == "server_name")
        _name = value;
      if (key == "index")
        _index = value;
      if (key == "method_accept")
        _methods = value;
      //else
      //std::cout << "Unknown key: " << key << " | value: " << value << std::endl;
    }
    i++;
  }
  std::cout << "END OF WHILE" << std::endl;
  if (_port == -1 || !_name.size() || !_index.size()) {
    std::cerr << "Error: Missing server informations" << std::endl;
    std::cerr << "Port: " << _port << std::endl
      << "Name: " << _name << std::endl
      << "index: " << _index << std::endl;
    exit(1);
  }
  _host = _name + ':' + _sport;
  _host.erase(std::remove(_host.begin(), _host.end(), ' '), _host.end());
  //if (location_conf.size())
  //std::cout << "Hanging location_cong: " << std::endl << location_conf << std::endl << "000000000000000" << std::endl;
  /* if (location && location_conf.size()) {
     loc.create_map(location_conf);
     _loc.push_back(loc);
     location_conf.clear();
     }*/
  //std::cout << "key:" << key << std::endl << "value:" << value << std::endl << std::endl;
}
/*
   void  Server::setup_server(std::string & config) {
//  std::cout << config << std::endl << "______________________" << std::endl;
std::istringstream stream(config);
std::string location_conf;
std::string line;
Location loc;
bool  location = false;

std::cout << "WHILE" << std::endl;
while (std::getline(stream, line)) {
std::cout << "LINE: " << line << std::endl;
std::istringstream line_stream(line);
std::string key;
std::string value;

std::getline(line_stream, key, ':');
std::getline(line_stream, value);

if (value.size())
trim(value);
//value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
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
if (key == "listen") {
_port = atoi(value.c_str());
_sport = value;
}
if (key == "server_name")
_name = value;
if (key == "index")
_index = value;
if (key == "method_accept")
_methods = value;
//else
//std::cout << "Unknown key: " << key << " | value: " << value << std::endl;
}
std::cout << "END OF WHILE" << std::endl;
if (_port == -1 || !_name.size() || !_index.size()) {
std::cerr << "Error: Missing server informations" << std::endl;
std::cerr << "Port: " << _port << std::endl
<< "Name: " << _name << std::endl
<< "index: " << _index << std::endl;
exit(1);
}
_host = _name + _sport;
//if (location_conf.size())
//std::cout << "Hanging location_cong: " << std::endl << location_conf << std::endl << "000000000000000" << std::endl;
if (location && location_conf.size()) {
loc.create_map(location_conf);
_loc.push_back(loc);
location_conf.clear();
}
//std::cout << "key:" << key << std::endl << "value:" << value << std::endl << std::endl;
}
}*/

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
    << "name :" << server._name << std::endl
    << "index :" << server._index << std::endl
    << "methods :" << server._methods << std::endl
    << "port :" << server._port << std::endl 
    << "string port :" << server._sport << std::endl 
    << "host :" << server._host << std::endl 
    << "_____________________________________" << std::endl << std::endl;

  int i = 1;
  for (unsigned long j = 0; j < server._loc.size(); j++) {
    nstream << "Location " << i++ << std::endl;
    nstream << server._loc[j] << std::endl
      << "---------------" << std::endl;
  }
  return nstream;
}
