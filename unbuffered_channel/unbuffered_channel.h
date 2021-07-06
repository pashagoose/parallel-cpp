#pragma once

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
      ++senders_pending_;
      sending_message_.notify_one();
      receiving_message_.wait(locker, [&] {
        return (receivers_pending_ > 0) || closed_; 
      });
      if (closed_) {
        throw std::runtime_error("Channel is closed");
      } else {
        val_ = value;
        --receivers_pending_;
        placed_val_ = true;
        placed_.notify_one();
      }
  }

  std::optional<T> Recv() {
    std::unique_lock locker(block_);
    sending_message_.wait(locker, [&] {
      return (senders_pending_ > 0) || closed_;
    });
    if (closed_) {
      return std::nullopt;
    }
    ++receivers_pending_;
    receiving_message_.notify_one();
    placed_.wait(locker, [&] {
      return placed_val_ || closed_;
    });
    if (closed_) {
      return std::nullopt;
    }
    --senders_pending_;
    placed_val_ = false;
    return val_;
  }

  void Close() {
    std::unique_lock locker(block_);
    closed_ = true;
    sending_message_.notify_all();
    receiving_message_.notify_all();
  }

 private:
  std::optional<T> val_;
  size_t receivers_pending_ = 0;
  size_t senders_pending_ = 0;
  bool closed_ = false;
  bool placed_val_ = false;
  std::mutex block_;
  std::condition_variable sending_message_;
  std::condition_variable receiving_message_;
  std::condition_variable placed_;
};

