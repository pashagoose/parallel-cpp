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
      std::unique_lock s_lock(sender_block_);
      std::unique_lock locker(all_block_);
      if (closed_) {
        throw std::runtime_error("Channel is closed");
      }
      placed_ = true;
      val_ptr_ = &value;
      ready_to_ship_.notify_one();
      grab_.wait(locker, [&] {
        return !placed_ || closed_;
      });
      if (placed_) {
        throw std::runtime_error("Channel is closed");
      }
  }

  std::optional<T> Recv() {
      std::unique_lock r_lock(receiver_block_);
      std::unique_lock locker(all_block_);
      ready_to_ship_.wait(locker, [&] {
        return placed_ || closed_;
      });
      if (closed_) {
        return std::nullopt;
      }
      placed_ = false;
      std::optional<T> ret_opt(*val_ptr_);
      grab_.notify_one();
      return ret_opt;
  }

  void Close() {
    std::unique_lock locker(all_block_);
    closed_ = true;
    ready_to_ship_.notify_all();
    grab_.notify_all();
  }

 private:
  T const *val_ptr_;
  bool closed_ = false;
  bool placed_ = false;
  std::mutex all_block_;
  std::mutex sender_block_;
  std::mutex receiver_block_;
  std::condition_variable ready_to_ship_;
  std::condition_variable grab_;

};

