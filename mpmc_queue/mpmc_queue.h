#pragma once

#include <atomic>
#include <cstdint>
#include <optional>
#include <thread>
#include <vector>

template <class T>
class MPMCQueue {
 public:
  explicit MPMCQueue(uint64_t max_size) : array_(max_size) {
    for (size_t i = 0; i < max_size; ++i) {
      array_[i].generation.store(i);
    }
  }

  bool Push(const T& value) {
    auto current_head = head_.load();
    for (;;) {
      auto index = current_head & (array_.size() - 1);
      if (array_[index].generation.load() != current_head) {
        if (current_head == tail_.load() + array_.size()) {
          return false;
        }
        std::this_thread::yield();
        current_head = head_.load();
      } else {
        if (!head_.compare_exchange_weak(current_head, current_head + 1)) {
          std::this_thread::yield();
          current_head = head_.load();
          continue;
        }
        array_[index].val.store(value);
        array_[index].generation.fetch_add(1);
        return true;
      }
    }
  }

  std::optional<T> Pop() {
    auto current_tail = tail_.load();
    for (;;) {
      auto index = current_tail & (array_.size() - 1);
      if (array_[index].generation.load() != current_tail + 1) {
        if (current_tail == head_.load()) {
          return std::nullopt;
        }
        std::this_thread::yield();
        current_tail = tail_.load();
      } else {
        if (!tail_.compare_exchange_weak(current_tail, current_tail + 1)) {
           std::this_thread::yield();
           current_tail = tail_.load();
           continue;
        }
        std::optional<T> ret_val(array_[index].val.load());
        array_[index].generation.fetch_add(array_.size() - 1);
        return ret_val;
      }
    }
  }

 private:
    
    struct Node {
      std::atomic<T> val;
      std::atomic<uint64_t> generation;

      Node() = default;
    };

    std::vector<Node> array_;
    std::atomic<uint64_t> head_ = 0;
    std::atomic<uint64_t> tail_ = 0;
};

