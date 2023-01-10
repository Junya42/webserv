#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include "../includes/macro.hpp"

int is_alpha(char c) {
  if (c >= 65 && c <= 90)
    return c + 32;
  else if (c >= 97 && c <= 122)
    return c - 32;
  else
    return 0;
}

bool comp(std::string &s, const char *str, size_t pos = 0, size_t i = 0, size_t j = 0, char c = 0, size_t len = 0) {
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
      return comp(s, str, pos, i + 1, j + 1, c, len);
    else {
      pos++;
      i = pos;
      if (len - j <= 1)
        return true;
      return comp(s, str, pos, i, 0, c, len);
      }
  }
  else {
    if (*(s.begin() + i) == str[j])
      return comp(s, str, pos, i + 1, j + 1, c, len);
    else {
      pos++;
      i = pos;
      if (len - j <= 1)
        return true;
      return comp(s, str, pos, i, 0, c, len);
    }
  }
  return true;
}

/*
 *  -----------------------------403067876422065921751058655471--
 *  ---------------------------403067876422065921751058655471
 */

bool comp(std::string &s, std::string &str, size_t pos = 0, size_t i = 0, size_t j = 0, char c = 0) {
  if (i == s.size() && j == str.size()) {
    return true;
  }
  if (j == str.size()) {
    return true;
  }
  if (i == s.size()) {
    return false;
  }
  if ((c = is_alpha(*(s.begin() + i))) > 0) {
    if (c == str[j] || *(s.begin() + i) == str[j])
      return comp(s, str, pos, i + 1, j + 1);
    else {
      if (str.size() - j <= 1)
        return true;
      pos++;
      i = pos;
      return comp(s, str, pos, i);
      }
  }
  else {
    if (*(s.begin() + i) == str[j])
      return comp(s, str, pos, i + 1, j + 1);
    else {
      //PRINT_LOG(s.c_str() + i);
      //PRINT_LOG(str.c_str() + j);
      //PRINT_LOG("Remaining size = " + std::to_string(str.size() - j));
      if (str.size() - j <= 1)
        return true;
      pos++;
      i = pos;
      return comp(s, str, pos, i);
    }
  }
  return true;
}

bool replace(std::string &s, const char *name, std::string &str, size_t found = 0) {
  std::string sub = name;

  found = s.find(sub, found);
  if (found != std::string::npos) {
    s.replace(found, sub.size(), str);
    return true;
  }
  return false;
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
