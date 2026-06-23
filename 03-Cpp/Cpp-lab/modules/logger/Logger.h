#pragma once

#include <string>

// Logger provides a single static helper so learning modules can emit
// diagnostics without pulling in a full logging framework.
class Logger {
public:
  static void log(const std::string &msg);
};