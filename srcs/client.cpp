#include "../includes/client.hpp"
#include <ios>
#include <ostream>
#include <unordered_map>

Client::Client(void) {
  _log = false;
  _sock = -1;
  _port = -1;
  _id = -1;
};

Client::~Client(void) {
};

void  Client::reset(void) {
  _sock = -1;
  _log = false;
  _port = -1;
  _addr.clear();
  _name.clear();
  _lastname.clear();
  _info.clear();
}

std::ostream &operator<<(std::ostream &nstream, Client &client) {
  nstream << std::endl << "Client info:" << std::endl << std::endl
    << "Socket: " << client._sock << std::endl
    << "Log: " << std::boolalpha << client._log << std::endl
    << "Port: " << client._port << std::endl
    << "Name: " << client._name << std::endl
    << "Last name: " << client._lastname << std::endl << std::endl
    << "Info: " << std::endl;

  for (std::unordered_map<std::string, std::string>::iterator it = client._info.begin(); it != client._info.end(); it++) {
    nstream << "(Key: " << it->first << " ) (Value: " << it->second << " )" << std::endl;
  }
  nstream << std::endl;
  return nstream;
}