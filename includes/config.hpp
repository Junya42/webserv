#ifndef CONFIG_HPP
# define CONFIG_HPP

#include <cstdlib>
#include "overload.hpp"
#include "server.hpp"
#include "string.hpp"
#include <ostream>
#include <map>

class Config {
  public:
    Config(void);
    ~Config(void);

    int  add_config(std::string &config);
    int  init_mime(void);

    std::vector<Server> _serv;
    std::map<std::string, std::string> _mime;
    std::string _pwd;

    unsigned long long connection_count;
	  unsigned long long request_count;
};

std::ostream &operator<<(std::ostream &nstream, Config &config);

#endif
