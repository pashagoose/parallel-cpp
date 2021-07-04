#pragma once

#include<chrono>
#include<mutex>
#include<thread>

class Deadlock {
 public:
  Deadlock() {
  }

  void ThreadOne() {
    mutex1.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mutex2.lock();
    mutex2.unlock();
    mutex1.unlock();
  }

  void ThreadTwo() {
    mutex2.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mutex1.lock();
    mutex1.unlock();
    mutex2.unlock();
  }

 private:
  std::mutex mutex1, mutex2;
};

