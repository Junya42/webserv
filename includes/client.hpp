#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <unordered_map>
#include <netinet/in.h>
#include "request.hpp"
#include <cstring>
#include "string.hpp"

class Client {
  public:
    Client(void);
    ~Client(void);

    void  reset(void);

    int addclient(int server, int epoll_fd, std::vector<Client> &clientlist);

    bool  _log;
    int _id;
    int _sock;
    unsigned short  _port;
    std::string _addr;
    std::string _name;
    std::string _lastname;
    std::unordered_map<std::string, std::string> _info;
    sockaddr_in addr;
    socklen_t addr_len;
    Request request;
};

std::ostream &operator<<(std::ostream &nstream, Client &client);

#endif
