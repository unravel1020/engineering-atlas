#pragma once

#include <iostream>
#include <mutex>
#include <ostream>
#include <string>

namespace logger {

// Log levels ordered from most verbose to most severe.
enum class Level { Debug = 0, Info = 1, Warning = 2, Error = 3 };

// Lightweight logger with level filtering and swappable output stream.
// It is intentionally simple: no external dependencies, no configuration files.
// For tests, the output stream can be redirected to a std::ostringstream.
class Logger {
public:
  static Logger &instance();

  // Set the minimum level that will actually be emitted.
  void setLevel(Level min_level);

  // Redirect output. Passing nullptr restores the default (std::cout).
  void setOutput(std::ostream *os);

  // Emit a log message if level >= min_level_. Thread-safe.
  void log(Level level, const std::string &message);

  void debug(const std::string &message);
  void info(const std::string &message);
  void warning(const std::string &message);
  void error(const std::string &message);

private:
  Logger();

  Level min_level_ = Level::Info;
  std::ostream *os_ = &std::cout;
  std::mutex mutex_;
};

// Convert a level enum to a human-readable prefix.
const char *levelToString(Level level);

} // namespace logger

#define LOG_DEBUG(msg) logger::Logger::instance().debug(msg)
#define LOG_INFO(msg) logger::Logger::instance().info(msg)
#define LOG_WARNING(msg) logger::Logger::instance().warning(msg)
#define LOG_ERROR(msg) logger::Logger::instance().error(msg)
