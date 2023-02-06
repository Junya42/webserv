#ifndef OVERLOAD_HPP
# define OVERLOAD_HPP

#include <vector>
#include <map>
#include <iostream>

template <typename T>
std::ostream &operator<<(std::ostream &n, std::vector<T> &vec) {
        n << std::endl;
        for (typename std::vector<T>::iterator it = vec.begin(); it != vec.end(); it++)
                n << "  " << *it << std::endl;
        return n;
}

template <typename A, typename B>
std::ostream &operator<<(std::ostream &n, std::map<A, B> &m) {
        n << std::endl;
        for (typename std::map<A, B>::iterator it = m.begin(); it != m.end(); it++)
                n << "  " << it->first << "  :  " << it->second << std::endl;
        return n;
}

#endif
