#include "pipeline/ThreadPool.h"

namespace oml {
namespace pipeline {

ThreadPool::ThreadPool(std::size_t num_threads) {
  for (std::size_t i = 0; i < num_threads; ++i) {
    workers_.emplace_back([this]() {
      for (;;) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(mutex_);
          condition_.wait(lock, [this]() { return stop_ || !tasks_.empty(); });
          if (stop_ && tasks_.empty()) {
            return;
          }
          task = std::move(tasks_.front());
          tasks_.pop();
        }
        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    stop_ = true;
  }
  condition_.notify_all();
  for (auto &worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

void ThreadPool::waitForAll() {
  // A simple wait strategy: submit an empty task per worker and wait for each.
  // This ensures all previously submitted tasks have been consumed.
  std::vector<std::future<void>> barriers;
  for (std::size_t i = 0; i < workers_.size(); ++i) {
    barriers.push_back(submit([]() {}));
  }
  for (auto &f : barriers) {
    f.wait();
  }
}

} // namespace pipeline
} // namespace oml
