#include <string>
#include <cstring>
#include <iostream>
#include "../includes/macro.hpp"

int is_alpha(char c) {
  if (c >= 65 && c <= 90)
    return c + 32;
  else if (c >= 97 && c <= 122)
    return c - 32;
  else
    return 0;
}

bool comp(std::string &s, const char *str, size_t i = 0, size_t j = 0, char c = 0, size_t len = 0) {
  if (len == 0)
    len = strlen(str);
  if (i == s.size() && j == len)
    return true;
  if (j == len)
    return true;
  if (i == s.size())
    return false;
  if ((c = is_alpha(*(s.begin() + i))) > 0) {
    if (c == str[j] || *(s.begin() + i) == str[j])
      return comp(s, str, i + 1, j + 1, c, len);
    else
      return comp(s, str, i + 1, 0, c, len);
  }
  else {
    if (*(s.begin() + i) == str[j])
      return comp(s, str, i + 1, j + 1, c, len);
    else
      return comp(s, str, i + 1, 0, c, len);
  }
  return true;
}

bool comp(std::string &s, std::string &str, size_t i = 0, size_t j = 0, char c = 0) {
  if (i == s.size() && j == str.size())
    return true;
  if (j == str.size())
    return true;
  if (i == s.size())
    return false;
  if ((c = is_alpha(*(s.begin() + i))) > 0) {
    if (c == str[j] || *(s.begin() + i) == str[j])
      return comp(s, str, i + 1, j + 1);
    else
      return comp(s, str, i + 1);
  }
  else {
    if (*(s.begin() + i) == str[j])
      return comp(s, str, i + 1, j + 1);
    else
      return comp(s, str, i + 1);
  }
  return true;
}

bool  erase(std::string &s, const char *str, int pos = -1, size_t i = 0, size_t j = 0, size_t len = 0) {
  if (len == 0)
    len = strlen(str);
  if (j == len) {
    s.erase(pos, j);
    return true;
  }
  if (i == s.size())
    return false;
  if (s[i] == s[j]) {
    if (pos == -1)
      pos = i;
    return erase(s, str, pos, i + 1, j + 1, len);
  }
  else {
    pos = -1;
    return erase(s, str, pos, i + 1, 0, len);
  }
  return true;
}
