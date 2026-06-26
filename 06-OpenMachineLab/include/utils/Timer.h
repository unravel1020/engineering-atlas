#pragma once

#include <chrono>
#include <cstdint>
#include <functional>

namespace oml {
namespace utils {

// High-resolution clock used throughout the framework for latency measurements.
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

// Return current timestamp in microseconds since epoch.
inline uint64_t nowUs() {
  return static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::microseconds>(
          Clock::now().time_since_epoch())
          .count());
}

// Stopwatch with explicit start/stop control.
class Stopwatch {
public:
  Stopwatch() { start(); }

  void start() { start_ = Clock::now(); }

  // Return elapsed microseconds since start(). Does not stop the watch.
  uint64_t elapsedUs() const {
    auto now = Clock::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(now - start_)
            .count());
  }

  // Return elapsed milliseconds since start().
  double elapsedMs() const { return static_cast<double>(elapsedUs()) / 1000.0; }

private:
  TimePoint start_;
};

// RAII timer that calls a callback with the elapsed milliseconds when destroyed.
class ScopedTimer {
public:
  using Callback = std::function<void(double)>;

  explicit ScopedTimer(Callback cb) : callback_(std::move(cb)) {}

  ~ScopedTimer() {
    if (callback_) {
      callback_(watch_.elapsedMs());
    }
  }

  // Prevent copying; allow moving.
  ScopedTimer(const ScopedTimer &) = delete;
  ScopedTimer &operator=(const ScopedTimer &) = delete;
  ScopedTimer(ScopedTimer &&) = default;
  ScopedTimer &operator=(ScopedTimer &&) = default;

private:
  Stopwatch watch_;
  Callback callback_;
};

} // namespace utils
} // namespace oml
