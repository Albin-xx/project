#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <iostream>
#include <sstream>

namespace client_server {
  size_t stringtosizet(std::string str);
  std::string sizettostring(size_t val);
}

#endif
