#ifndef STRING_HPP
# define STRING_HPP

#include <string>

int   is_alpha(char c);

bool  comp(std::string &s, const char *str, size_t i = 0, size_t j = 0, char c = 0, size_t len = 0);

bool  comp(std::string &s, std::string &str, size_t i = 0, size_t j = 0, char c = 0);

#endif
