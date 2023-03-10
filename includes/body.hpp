#ifndef BODY_HPP
# define BODY_HPP

#include <string>
#include "string.hpp"
#include <vector>

class Body {
  public:

    Body(void) {
    }
    ~Body(void) {
    }
    void clear(void) {
      disposition.clear();
      filename.clear();
      type.clear();
      data.clear();
    }
    std::string disposition;
    std::string filename;
    std::string type;
    std::vector<std::string> data;
};

#endif
