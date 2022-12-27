#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <iostream>
#include <unordered_map>

class Client {
  public:
    Client(void);
    ~Client(void);

    void  reset(void);

    bool  _log;
    int _id;
    int _sock;
    unsigned short  _port;
    std::string _addr;
    std::string _name;
    std::string _lastname;
    std::unordered_map<std::string, std::string> _info;
};

std::ostream &operator<<(std::ostream &nstream, Client &client);

#endif
