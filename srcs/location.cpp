#include "../includes/location.hpp"
#include <sstream>
#include <iostream>
#include <map>

Location::Location(void) {
}

Location::~Location(void) {

}

Location::Location(std::string &config) {
  std::istringstream stream(config);
  std::string line;

  while (std::getline(stream, line) && line != "\r") {
    std::istringstream line_stream(line);
    std::string key;
    std::string value;
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);
    while (value[0] == '\t' || value[0] == ' ')
      value.erase(0, 1);
    _data[key] = value;
  }
}

Location &Location::operator=(const Location &src) {
  _data = src._data;
  return *this;
}

void  Location::create_map(std::string & config) {
  _data.clear();
  method.clear();
  _root.clear();
  _path.clear();
  _link.clear();
  _index.clear();
  _upload.clear();
  _mbsize = -1;
  _redirect = false;
   _autoindex = false;
  std::istringstream stream(config);
  std::string line;

  while (std::getline(stream, line)) {
    std::istringstream line_stream(line);
    std::string key;
    std::string value;
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);
    if (value.size()) {
      while (value[0] == '\t' || value[0] == ' ')
        value.erase(0, 1);
      _data[key] = value;
    }
    if (key.find("root") != std::string::npos) {
      _root = value;
    }
    else if (key.find("set_index") != std::string::npos) {
      _index = value;
    }
    else if (key.find("auto_index") != std::string::npos) {
      if (value == "true")
        _autoindex = true;
    }
    else if (key.find("max_body_size") != std::string::npos) {
      _mbsize = strtoll(value.c_str(), NULL, 10);
    }
    else if (key.find("location") != std::string::npos) {
      _path = value;
    }
    else if (key.find("set_upload") != std::string::npos) {
      _upload = value;
    }
    else if (key.find("method_accept") != std::string::npos) {
      std::istringstream methods(value);
      std::string tmp;
      while (std::getline(methods, tmp, ' ')) {
        method[tmp] = true;
      }
    }
    else if (key.find("redirect") != std::string::npos) {
      _redirect = true;
      _link = value;
      
      while (_link.size() && (_link[0] == ' ' || _link[0] == '\t'))
        _link.erase(0, 1);
    }
  }
}

std::ostream &operator<<(std::ostream &nstream, Location &loc) {
  for (std::map<std::string, std::string>::iterator it = loc._data.begin(); it != loc._data.end(); it++) {
    if (it->first != "location")
      nstream << "  " << it->first << " :::: " << it->second << std::endl;
  }
  return nstream;
}
