#include "../includes/server.hpp"
#include "../includes/overload.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <sys/types.h>

const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

Server::Server(void) {
  _redirect = false;
  _login = false;
  _port = -1;
}

void  Server::clear(void) {
  _host.clear();
  _name.clear();
  _sport.clear();
  _index.clear();
  _cgi.clear();
  errors.clear();
  _methods.clear();
  _redirect = false;
  _login = false;
  _get = false;
  _post = false;
  _delete = false;
  _bodysize = 0;
  _port = 0;
  _valid = false;
  _loc.clear();
}

void  Server::setup_server(std::vector<std::string> &vec) {
//  std::cout << config << std::endl << "______________________" << std::endl;
//std::cout << std::endl << "__________________________________________" << std::endl;
  std::string location_conf;
  std::string line;
  Location loc;
  int location_push = 0;
  int location_find = 0;
  bool  location = false;

  long unsigned int i = 0;
  while (i < vec.size()) {
    line = vec[i];
   // std::cout << "LINE: " << line << std::endl;
    std::istringstream line_stream(line);
    std::string key;
    std::string value;

    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);

    if (value.size())
      trim(value);
    //value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
    if (key == "location") {
      location_find++;
      if (location == true) {
        location_push++;
        loc.create_map(location_conf);
        _loc.push_back(loc);
        // std::cout << std::endl << "LOCATION CONF" << std::endl << location_conf << "END OF LOCATION CONF" <<std::endl;
      }
      location = true;
      location_conf.clear();
    }
    if (location) {
      location_conf += line;
      location_conf += '\n';
    }
    else {
     // std::cout << "key: " << key << std::endl;
      if (key == "listen") {
        if (value.size() > 6 || value.size() < 1) {
          PRINT_ERR("Error Port size");
          exit(0);
        } 
        for (std::string::iterator it = value.begin(); it != value.end(); it++) {
          if (!isdigit(*it) && *it != ' ') {
            PRINT_ERR("Error Port does not contain only digits");
            PRINT_ERR(value);
            exit(0);
          }
        }
        _port = atoi(value.c_str());
        _sport = value;
      }
      if (key == "error_pages") {
        if (value.size() && value[0] == ' ')
          erase(value, " ");
        std::istringstream errorparser(value);
        std::string errKey;
        std::string errValue;
        while (std::getline(errorparser, errKey, ' ')) {
          std::getline(errorparser, errValue, ' ');
          errors[atoi(errKey.c_str())] = errValue;
        }
        //std::cout << "Errors" << errors << std::endl;
      }
      if (key == "server_name")
        _name = value;
      if (key == "index")
        _index = value;
      if (key == "method_accept")
        _methods = value;
      if (key == "force_login") {
        if (value == " true")
          _login = true;
      }
      if (key == "enable_redirect") {
        if (value == " true")
          _redirect = true;
      }
      if (key == "cgi") {
        _cgi = value;
        erase(value, " ");
        std::istringstream cgiparser(value);
        std::string tmpkey;
        std::string tmpvalue;
        while (std::getline(cgiparser, tmpkey, ' ')) {
          std::getline(cgiparser, tmpvalue, ' ');
          cgi[tmpkey] = tmpvalue;
        }
      }
      //else
      //std::cout << "Unknown key: " << key << " | value: " << value << std::endl;
    }
    i++;
  }
  //if (_port == -1 || !_name.size() || !_index.size()) {
  if (_port == -1 || !_name.size()) {
    if (_port == -1)
      PRINT_ERR("Port error");
    if (!_name.size())
      PRINT_ERR("Name error");
    PRINT_ERR("Error server config");
    exit(1);
  }
  struct in_addr a;
  struct hostent *lh = gethostbyname(_name.substr(1).c_str());
  if (lh) {
    bcopy(*lh->h_addr_list, (char *)&a, sizeof(a));
    _ip = to_string(inet_ntoa(a));
  }
  else
    _ip = "error";
  _host = _name + ':' + _sport;
  _host.erase(std::remove(_host.begin(), _host.end(), ' '), _host.end());
  if (location_find != location_push) {
        loc.create_map(location_conf);
        _loc.push_back(loc);
  }
  _valid = true;
 // std::cout << "___________________________________________________" << std::endl;
}

Server::Server(std::string &config) {
  std::cout << "config: " << config << std::endl << std::endl;
  std::istringstream stream(config);
  std::string location_conf;
  std::string line;
  bool  location = false;

  while (std::getline(stream, line)) {
    std::istringstream line_stream(line);
    std::string key;
    std::string value;
    std::getline(line_stream, key, ':');
    std::getline(line_stream, value);
    if (key == "location") {
      if (location == true) {
        _loc.push_back(Location(location_conf));
      }
      location = true;
      location_conf.clear();
    }
    if (location)
      location_conf += line;
    else {
      if (key == "listen")
        _port = atoi(value.c_str());
      else if (key == "server_name")
        _name = value;
      else if (key == "index")
        _index = value;
      else if (key == "method_accept")
        _methods = value;
    }
  }
}

Server::~Server(void) {
}

std::string &Server::get_name(void) {
  return _name;
}

std::string &Server::get_index(void) {
  return _index;
}

ssize_t Server::get_size(void) {
  return _bodysize;
}

int Server::get_port(void) {
  return _port;
}

bool  Server::method_get(void) {
  return _get;
}

bool  Server::method_post(void) {
  return _post;
}

bool  Server::method_delete(void) {
  return _delete;
}

std::ostream &operator<<(std::ostream &nstream, Server &server) {
    nstream << std::boolalpha << "\033[36mname: \033[0m" << server._name << std::endl
    << "\033[36mindex: \033[0m" << server._index << std::endl
    << "\033[36mcgi: \033[0m" << server._cgi << std::endl
    << "\033[36mforce login: \033[0m" << server._login << std::endl
    << "\033[36menable redirect: \033[0m" << server._redirect << std::endl
    << "\033[36mport: \033[0m" << server._port << std::endl 
    << "\033[36mstring port: \033[0m" << server._sport << std::endl 
    << "\033[36mhost: \033[0m" << server._host << std::endl
    << "\033[36merrors: \033[0m" << server.errors << std::endl
    << "_____________________________________" << std::endl << std::endl;

  int i = 1;
  for (unsigned long j = 0; j < server._loc.size(); j++) {
    nstream << "\033[38;5;49mLocation " << i++ << "\033[0m" << std::endl;
    nstream << server._loc[j] << std::endl;
    if (j + 1 < server._loc.size())
      nstream << "---------------" << std::endl;
  }
  return nstream;
}
