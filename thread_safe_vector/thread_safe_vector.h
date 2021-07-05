#pragma once

#include <mutex>
#include <shared_mutex>
#include <vector>


template <typename T>
class ThreadSafeVector {
 public:
  ThreadSafeVector() {
  }

  T operator[](size_t index) const {
    std::shared_lock r_lock(block_);
    return vector_[index];
  }

  size_t Size() const {
    std::shared_lock r_lock(block_);
    return vector_.size();
  }

  void PushBack(const T& value) {
    std::unique_lock w_lock(block_);
    vector_.push_back(value);
  }

 private:
  mutable std::shared_mutex block_;
  std::vector<T> vector_;
};

