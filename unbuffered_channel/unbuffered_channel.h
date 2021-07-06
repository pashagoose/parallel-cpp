#pragma once

#include <cassert>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <optional>


template <typename T>
class UnbufferedChannel{
 public:
  UnbufferedChannel() {
  }

  void Send(const T& value) {
      std::unique_lock locker(block_);
      ready_to_get_.wait(locker, [&] {
        return (closed_ || receivers_pending_);
      });
      if (closed_) {
        throw std::runtime_error("Channel is closed");
      } else {
        assert(!placed_);
        placed_ = true;
        val_ = value;
        placed_val_.notify_one();
      }
  }

  std::optional<T> Recv() {
      std::unique_lock locker(block_);
      ++receivers_pending_;
      ready_to_get_.notify_one();
      placed_val_.wait(locker, [&] {
        return (closed_ || placed_);
      });
      if (closed_) {
        return std::nullopt;
      } else {
        placed_ = false;
        --receivers_pending_;
        return val_;
      }
  }

  void Close() {
    std::unique_lock locker(block_);
    closed_ = true;
    placed_val_.notify_all();
    ready_to_get_.notify_all();
  }

 private:
  std::optional<T> val_;
  size_t receivers_pending_ = 0;
  bool closed_ = false;
  bool placed_ = false;
  std::mutex block_;
  std::condition_variable placed_val_;
  std::condition_variable ready_to_get_;
};

