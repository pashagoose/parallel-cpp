#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>


template <typename T>
class ThreadSafeQueue {
 public:
  ThreadSafeQueue() {
  }

  void Push(const T& value) {
    std::unique_lock locker(block_);
    queue_.push(value);
    not_empty_condition_.notify_all();
  }

  T Pop() {
    std::unique_lock locker(block_);
    not_empty_condition_.wait(locker, [&] {
      return !queue_.empty();
    });
    auto value = queue_.front();
    queue_.pop();
    return value;
  }

  std::optional<T> TryPop() {
    std::unique_lock locker(block_);
    if (queue_.empty()) {
      return std::nullopt;
    }
    auto value = queue_.front();
    queue_.pop();
    return value;
  }

 private:
  std::mutex block_;
  std::condition_variable not_empty_condition_;
  std::queue<T> queue_;
};

