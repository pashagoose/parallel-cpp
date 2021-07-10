#pragma once

#include <atomic>
#include <thread>

class RWSpinLock {
 public:
  RWSpinLock() {
  }

  void LockRead() {
    for (;;) {
      auto copy_cnt = counter_.load();
      if (copy_cnt & 1) {
        std::this_thread::yield();
        continue;
      }
      if (counter_.compare_exchange_weak(copy_cnt, copy_cnt + 2)) {
        break;
      }
    }
  }

  void UnlockRead() {
    for (;;) {
      auto copy_cnt = counter_.load();
      if (counter_.compare_exchange_weak(copy_cnt, copy_cnt - 2)) {
        break;
      }
    }
  }

  void LockWrite() {
    for (;;) {
      auto copy_cnt = counter_.load();
      if (copy_cnt & 1) {
        std::this_thread::yield();
        continue;
      }
      if (counter_.compare_exchange_weak(copy_cnt, copy_cnt + 1)) {
        break;
      }
    }
    while (counter_.load() != 1) {
      std::this_thread::yield();
    }
  }

  void UnlockWrite() {
    counter_.store(0);
  }

 private:
  std::atomic<uint64_t> counter_ = 0;
};

