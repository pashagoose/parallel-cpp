#pragma once

#include <atomic>
#include <thread>

class TicketLock {
 public:
  TicketLock() {
  }

  void Lock() {
    size_t cur_id = tickets_cnt_.fetch_add(1);
    while (now_serving_.load() != cur_id) {
      std::this_thread::yield();
    }
  }

  void Unlock() {
    now_serving_.fetch_add(1);
  }

 private:
  std::atomic<size_t> tickets_cnt_ = 0;
  std::atomic<size_t> now_serving_ = 0;
};

