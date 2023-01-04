#include "../includes/mime.hpp"
#include "../includes/macro.hpp"
#include "../includes/config.hpp"

std::string get_mime(std::string &filename, std::map<std::string,std::string> &_mime) {
  std::string mimevalue;
  std::string extension;
  size_t extension_pos;

  extension_pos = filename.find_last_of(".");
  extension = filename.substr(extension_pos);
  PRINT_LOG(extension);
  mimevalue = _mime[extension];
  PRINT_LOG(mimevalue);
  return mimevalue;
}
