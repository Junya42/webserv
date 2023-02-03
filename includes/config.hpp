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

    void  add_config(std::string &config);
    void  init_mime(void);

    std::vector<Server> _serv;
    std::map<std::string, std::string> _mime;
    std::string _pwd;
};

std::ostream &operator<<(std::ostream &nstream, Config &config);

#endif
