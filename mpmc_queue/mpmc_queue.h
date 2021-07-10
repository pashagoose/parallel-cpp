#pragma once

#include <atomic>
#include <cstdint>
#include <optional>
#include <thread>
#include <vector>

template <class T>
class MPMCQueue {
 public:
  explicit MPMCQueue(uint64_t max_size) : array_(2 * max_size) {
    // initializing array_ with 2 * max_size, because cannot resize it
    // atomic is not copyable
    uint64_t good_size = 1;
    while (good_size < max_size) {
      good_size *= 2;
    }
    abstract_size_ = good_size;
    for (size_t i = 0; i < good_size; ++i) {
      array_[i].generation.store(i);
    }
  }

  bool Push(const T& value) {
    auto current_head = head_.load();
    for (;;) {
      auto index = current_head & (abstract_size_ - 1);
      if (array_[index].generation.load() != current_head) {
        if (current_head == tail_.load() + abstract_size_) {
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
      auto index = current_tail & (abstract_size_ - 1);
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
        array_[index].generation.fetch_add(abstract_size_ - 1);
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

    uint64_t abstract_size_ = 1;
    std::vector<Node> array_;
    std::atomic<uint64_t> head_ = 0;
    std::atomic<uint64_t> tail_ = 0;
};

