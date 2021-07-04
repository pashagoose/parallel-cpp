#pragma once

#include<chrono>
#include<mutex>
#include<thread>

class Deadlock {
 public:
  Deadlock() {
  }

  void ThreadOne() {
    mutex1_.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mutex2_.lock();
    mutex2_.unlock();
    mutex1_.unlock();
  }

  void ThreadTwo() {
    mutex2_.lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mutex1_.lock();
    mutex1_.unlock();
    mutex2_.unlock();
  }

 private:
  std::mutex mutex1_, mutex2_;
};

