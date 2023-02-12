#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <vector>
#include <sys/types.h>
#include "location.hpp"
#include "string.hpp"
#include <cstdio>
#include <netdb.h>
#include <cstring>
#include <arpa/inet.h>

class Location;

class Server {
  public:
    Server(void);
    Server(std::string &config);
    ~Server(void);
    //void  setup_server(std::string & config);
    int  setup_server(std::vector<std::string> &vec);
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
    std::string _ip;
    std::string _cgi;
    bool        _large;
    bool        _valid;
    bool        _get;
    bool        _post;
    bool        _delete;
    bool        _login;
    bool        _redirect;
    ssize_t     _bodysize;
    int         _port;
    int         _sock;
    unsigned long long  _connection;
    unsigned long long _requests;
    std::vector<Location> _loc;
    std::map<std::string, std::string> cgi;
    std::map<int, std::string> errors;
};

std::ostream &operator<<(std::ostream &nstream, Server &server);

#endif
