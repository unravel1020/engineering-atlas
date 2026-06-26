#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace oml {
namespace pipeline {

// Thread-safe unbounded queue used between pipeline stages. A nullptr sentinel
// value signals the producer has finished. Unbounded sizing avoids shutdown
// deadlocks in this learning-stage implementation.
template <typename T> class BlockingQueue {
public:
  BlockingQueue() = default;

  void push(T value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (stop_) {
      return;
    }
    queue_.push(std::move(value));
    not_empty_.notify_one();
  }

  // Returns std::nullopt if the queue is stopped and empty.
  std::optional<T> pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    not_empty_.wait(lock, [this]() { return !queue_.empty() || stop_; });
    if (queue_.empty()) {
      return std::nullopt;
    }
    T value = std::move(queue_.front());
    queue_.pop();
    return value;
  }

  void stop() {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      stop_ = true;
    }
    not_empty_.notify_all();
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable not_empty_;
  bool stop_ = false;
};

} // namespace pipeline
} // namespace oml
