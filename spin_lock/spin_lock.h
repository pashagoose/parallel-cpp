#pragma once

#include<atomic>
#include<thread>

class SpinLock {
 public:
  SpinLock() {
  }

  void Lock() {
    while (locked.exchange(true)) {
      // me, waiting for toilet
      std::this_thread::yield();
    }
  }

  void Unlock() {
    locked.store(false);
  }

 private:
  std::atomic<bool> locked = false;
};

