#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace utils {

// Reads an entire text file into a string. Throws if the file cannot be opened.
// This is a thin wrapper so callers do not repeat ifstream boilerplate.
inline std::string readTextFile(const std::string &path) {
  std::ifstream ifs(path);
  if (!ifs.is_open()) {
    throw std::runtime_error("failed to open file: " + path);
  }

  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}

// Loads a newline-delimited label file. Empty lines are preserved so that line
// numbers stay aligned with class indices, which matters for models where some
// class IDs intentionally have no human-readable name.
inline std::vector<std::string> loadLabels(const std::string &path) {
  std::ifstream ifs(path);
  if (!ifs.is_open()) {
    throw std::runtime_error("failed to open labels file: " + path);
  }

  std::vector<std::string> labels;
  std::string line;
  while (std::getline(ifs, line)) {
    labels.push_back(line);
  }
  return labels;
}

// Joins path components with a single '/' separator. This is a minimal helper
// for model/config paths; it does not normalize '.' or '..' segments.
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
