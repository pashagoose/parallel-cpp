#pragma once

#include <optional>
#include <cstdint>


template <class T>
class MPMCQueue {
 public:
  explicit MPMCQueue(uint64_t max_size) {
    uint64_t good_size = 1;
    while (good_size < max_size) {
      good_size *= 2;
    }
    array.resize(good_size);
  }

  bool Push(const T& /*value*/) {
    // Your code
    return false;
  }

  std::optional<T> Pop() {
    // Your code
    return std::nullopt;
  }

 private:
    
    struct Node {
      std::atomic<T> value;
      std::atomic<uint64_t> generation;
    }

    std::vector<Node> array_;
    std::atomic<uint64_t> head_ = static_cast<uint64_t> (-1);
    std::atomic<uint64_t> tail_ = 0;
};

