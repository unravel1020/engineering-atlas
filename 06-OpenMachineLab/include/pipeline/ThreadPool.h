#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace oml {
namespace pipeline {

// Minimal fixed-size thread pool. Used to execute pipeline stages concurrently
// across frames. The first version does not support work stealing or priority.
class ThreadPool {
public:
  explicit ThreadPool(std::size_t num_threads);
  ~ThreadPool();

  // Submit a task and return a future for its result.
  template <typename F, typename... Args>
  auto submit(F &&f, Args &&...args)
      -> std::future<typename std::invoke_result<F, Args...>::type> {
    using ReturnType = typename std::invoke_result<F, Args...>::type;

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<ReturnType> result = task->get_future();
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (stop_) {
        throw std::runtime_error("cannot submit task to stopped ThreadPool");
      }
      tasks_.emplace([task]() { (*task)(); });
    }
    condition_.notify_one();
    return result;
  }

  // Wait for all submitted tasks to complete.
  void waitForAll();

  std::size_t size() const { return workers_.size(); }

private:
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;

  std::mutex mutex_;
  std::condition_variable condition_;
  bool stop_ = false;
};

} // namespace pipeline
} // namespace oml
