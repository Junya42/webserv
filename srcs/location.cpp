#include "../includes/location.hpp"
#include <sstream>
#include <unordered_map>

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
    _data[key] = value;
  }
}

Location &Location::operator=(const Location &src) {
  _data = src._data;
  return *this;
}

void  Location::create_map(std::string & config) {
  _data.clear();
  std::istringstream stream(config);
  std::string line;

  while (std::getline(stream, line) && line != "\r") {
    std::istringstream line_stream(line);
    std::string key;
    std::string value;
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);
    if (value.size())
      _data[key] = value;
  }
}

std::ostream &operator<<(std::ostream &nstream, Location &loc) {
  for (std::unordered_map<std::string, std::string>::iterator it = loc._data.begin(); it != loc._data.end(); it++) {
    nstream << "  " << it->first << " :::: " << it->second << std::endl;
  }
  return nstream;
}
