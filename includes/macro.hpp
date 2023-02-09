#ifndef MACRO_HPP
# define MACRO_HPP

#include <iostream>
#include <string.h>

  # define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

  # define PRINT_LOG(x) std::cout << "[\033[1;36m" << __FUNCTION__ << "\033[0m] line : "\
    << __LINE__ << " [" << x << "]" << std::endl;

  # define PRINT_ERR(x) std::cerr << "[\033[1;31m" << __FUNCTION__ << "\033[0m] line : "\
    << __LINE__ << " [" << x << "]" << std::endl;

  # define PRINT_WIN(x) std::cout << "[\033[1;32m" << __FUNCTION__ << "\033[0m] line : "\
    << __LINE__ << " [" << x << "]" << std::endl;

  # define PRINT_FUNC() std::cout << "[\033[36m" << __FUNCTION__ << "\033[0m] "\
    << __FILENAME__ << std::endl;

#endif