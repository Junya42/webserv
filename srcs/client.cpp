#include "../includes/client.hpp"
#include <ios>
#include <ostream>
#include <map>

Client::Client(void) {
  _log = true;
  _fav = false;
  _sock = -1;
  _port = -1;
  _id = -1;
  _request_count = 0;
  _addr.clear();
  _name.clear();
  _lastname.clear();
  _cookie.clear();
  _info.clear();
  std::memset(&addr, 0, sizeof(addr));
  request.clear();
  addr_len = sizeof(addr);
};

Client::~Client(void) {
};

void  Client::reset(void) {
  _sock = -1;
  _log = true;
  _fav = false;
  _port = -1;
  _request_count = 0;
  _addr.clear();
  _name.clear();
  _lastname.clear();
  _cookie.clear();
  _info.clear();
  std::memset(&addr, 0, sizeof(addr));
  request.clear();
}

std::ostream &operator<<(std::ostream &nstream, Client &client) {
  nstream << std::endl << "Client info:" << std::endl << std::endl
    << "Socket: " << client._sock << std::endl;
  if (client._log == true)
    nstream << "Log: \033[1;32m" << std::boolalpha << client._log << "\033[0m" << std::endl;
  else
    nstream << "Log: \033[1;31m" << std::boolalpha << client._log << "\033[0m" << std::endl;
  if (client._fav == true)
    nstream << "Fav: \033[1;32m" << std::boolalpha << client._fav << "\033[0m" << std::endl;
  else
    nstream << "Fav: \033[1;31m" << std::boolalpha << client._fav << "\033[0m" << std::endl;
  nstream << "Request count: " << client._request_count << std::endl
    << "Port: " << client._port << std::endl
    << "Host: " << client._host << std::endl
    << "Address: " << inet_ntoa(client.addr.sin_addr) << std::endl
    << "Cookie: " << client._cookie << std::endl
    << "Name: " << client._name << std::endl
    << "Query: " << client._lastname << std::endl << std::endl
    << "Files: " << std::endl;

  for (std::vector<std::string>::iterator it = client._files.begin(); it != client._files.end(); it++) {
    nstream << "  " << *it << std::endl;
  }
  nstream << std::endl;
  return nstream;
}
