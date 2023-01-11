#ifndef STRING_HPP
# define STRING_HPP

#include <string>
#include <sstream>
#include "macro.hpp"

int   is_alpha(char c);

bool  comp(std::string &s, const char *str, size_t pos = 0, size_t i = 0, size_t j = 0, char c = 0, size_t len = 0);

bool  comp(std::string &s, std::string &str, size_t pos = 0, size_t i = 0, size_t j = 0, char c = 0);

bool  replace(std::string &s, const char *name, std::string &str, size_t found = 0);

bool  erase(std::string &s, const char *str, int pos = -1, size_t i = 0, size_t j = 0, size_t len = 0);

void erase(std::string &s, char c);

void erase(std::string &s);

template <typename T>
std::string to_string(const T src) {
  std::ostringstream s;
  std::string tmp;

  s << src;
  tmp = s.str();
  return tmp;
}

#endif
