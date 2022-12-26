#include "../includes/client.hpp"

Client::Client(void) {
  _log = false;
};

Client::~Client(void) {
};

void  Client::reset(void) {
  _sock = 0;
  _log = false;
  _port = 0;
  _addr.clear();
  _name.clear();
  _lastname.clear();
  _info.clear();
}
