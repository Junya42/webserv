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
  if (str[j] == '*') {  
    j++;
    while (s[i] && s[i] != str[j])
      i++;
    if (i == s.size())
      return false;
    if (s[i] != str[j])
      return false;
  }
  if ((c = is_alpha(*(s.begin() + i))) > 0) {
    if (c == str[j] || *(s.begin() + i) == str[j])
      return comp(s, str, pos, i + 1, j + 1, c, len);
    else {
      pos++;
      i = pos;
      if (len - j <= 1 && (str[j] == 13 || str[j] == 10))
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
      if (len - j <= 1 && (str[j] == 13 || str[j] == 10))
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
      if (str.size() - j <= 1 && (str[j] == 13 || str[j] == 10))
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
      if (str.size() - j <= 1 && (str[j] == 13 || str[j] == 10))
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

void  erase(std::string &s, char c) {
  for (size_t i = 0; i < s.size(); i++)
    if (s[i] == c)
      s.erase(i, 1);
}

void erase(std::string &s) {

  size_t  pos = 0;
  bool    found = false;

  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == '/') {
      pos = i;
      found = true;
    }
    if (isalnum(s[i]) < 1 && s[i] != '.' && s[i] != '-' && s[i] != '_')
      s.erase(i, 1);
    else if (s[i] == '\r' || s[i] == '\n' || s[i] == 10 || s[i] == 13)
      s.erase(i, 1);
  }
  if (found == true)
    for (size_t i = 0; i <= pos; i++)
      s.erase(i, 1);
}

size_t  find(std::string &s, char c, size_t iter = 1, size_t count = 0, size_t i = 0) {
  while (s[i]) {
    if (s[i] == c)
      count++;
    if (count == iter)
      return i;
    i++;
  }
  return i;
}
