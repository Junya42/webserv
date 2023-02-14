#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <ostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "string.hpp"
#include <map>

class Location {
  public:
    Location(void);
    Location(std::string &config);
    Location &operator=(const Location &src);
    ~Location(void);
    void  create_map(std::string &config);
    
    std::map<std::string, std::string> _data;
    std::map<std::string, bool> method;
    std::string _index;
    std::string _path;
    std::string _root;
    std::string _link;
    std::string _upload;
    long long int _mbsize;
    bool  _autoindex;
    bool  _redirect;


};

std::ostream &operator<<(std::ostream &nstream, Location &loc);

#endif
