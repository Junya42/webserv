#ifndef MIME_HPP
# define MIME_HPP

#include <map>
#include <string>

std::string get_mime(std::string &extension, std::map<std::string, std::string> &config);

#endif
