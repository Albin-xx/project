#include "util.h"
#include <string>
#include <iostream>
#include <sstream>
namespace client_server {
  size_t stringtosizet(std::string str) {
    size_t val;
    std::istringstream iss(str);
    iss >> val;
    return val;
  }

  std::string sizettostring(size_t val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
  }

}
