#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <vector>
#include <sys/types.h>
#include "location.hpp"
#include "string.hpp"

class Location;

class Server {
  public:
    Server(void);
    Server(std::string &config);
    ~Server(void);
    //void  setup_server(std::string & config);
    void  setup_server(std::vector<std::string> &vec);
    void  clear(void);
    std::string   &get_name(void);
    std::string   &get_index(void);
    bool          method_get(void);
    bool          method_post(void);
    bool          method_delete(void);
    ssize_t       get_size(void);
    int           get_port(void);
    std::vector<Location> &get_locations(void);

    std::string _host;
    std::string _name;
    std::string _sport;
    std::string _index;
    std::string _methods;
    bool        _get;
    bool        _post;
    bool        _delete;
    ssize_t     _bodysize;
    int         _port;
    std::vector<Location> _loc;
};

std::ostream &operator<<(std::ostream &nstream, Server &server);

#endif
