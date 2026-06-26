#include "logger.h"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace logger {

namespace {

// Current local time as HH:MM:SS. Used for simple timestamping without
// pulling in <chrono> formatting complexity.
std::string currentTimeString() {
  auto now = std::time(nullptr);
  auto tm = std::localtime(&now);
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << tm->tm_hour << ":"
      << std::setw(2) << tm->tm_min << ":" << std::setw(2) << tm->tm_sec;
  return oss.str();
}

} // namespace

Logger &Logger::instance() {
  static Logger logger;
  return logger;
}

Logger::Logger() = default;

void Logger::setLevel(Level min_level) { min_level_ = min_level; }

void Logger::setOutput(std::ostream *os) { os_ = os ? os : &std::cout; }

void Logger::log(Level level, const std::string &message) {
  if (level < min_level_) {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  *os_ << "[" << currentTimeString() << "] " << levelToString(level) << " "
       << message << "\n";
}

void Logger::debug(const std::string &message) { log(Level::Debug, message); }
void Logger::info(const std::string &message) { log(Level::Info, message); }
void Logger::warning(const std::string &message) { log(Level::Warning, message); }
void Logger::error(const std::string &message) { log(Level::Error, message); }

const char *levelToString(Level level) {
  switch (level) {
  case Level::Debug:
    return "[DEBUG]";
  case Level::Info:
    return "[INFO] ";
  case Level::Warning:
    return "[WARN] ";
  case Level::Error:
    return "[ERROR]";
  }
  return "[UNKNOWN]";
}

} // namespace logger
