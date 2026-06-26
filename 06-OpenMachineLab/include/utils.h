#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace oml {
namespace utils {

inline std::string readTextFile(const std::string &path) {
  std::ifstream ifs(path);
  if (!ifs.is_open()) {
    throw std::runtime_error("failed to open file: " + path);
  }
  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}

inline std::string joinPath(const std::string &a, const std::string &b) {
  if (a.empty()) {
    return b;
  }
  if (b.empty()) {
    return a;
  }
  if (a.back() == '/') {
    return a + b;
  }
  return a + "/" + b;
}

} // namespace utils
} // namespace oml
