#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <ostream>
#include <string>
#include <sstream>
#include "string.hpp"
#include <unordered_map>

class Location {
  public:
    Location(void);
    Location(std::string &config);
    Location &operator=(const Location &src);
    ~Location(void);
    void  create_map(std::string &config);
    
    std::unordered_map<std::string, std::string> _data;
    std::string _path;
    std::string _root;
};

std::ostream &operator<<(std::ostream &nstream, Location &loc);

#endif
