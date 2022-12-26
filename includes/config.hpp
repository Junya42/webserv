#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "client.hpp"
#include "server.hpp"
#include <ostream>

class Config {
  public:
    Config(void);
    ~Config(void);
    void  add_config(std::string &config);
    std::vector<Server> _serv;
};

std::ostream &operator<<(std::ostream &nstream, Config &config);

#endif
