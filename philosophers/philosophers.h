#pragma once

#include <mutex>


class Fork {
 public:
  Fork(size_t id) : id_(id) {
  }

  size_t Id() const {
    return id_;
  }

  void Get() {
    mutex_.lock();
  }

  bool TryGet() {
    return mutex_.try_lock();
  }

  void Put() {
    mutex_.unlock();
  }

 private:
  size_t id_;
  std::mutex mutex_;
};

class Philosopher {
 public:
  Philosopher(size_t id, Fork* left_fork, Fork* right_fork) :
                id_(id),
                lfork_(left_fork),
                rfork_(right_fork)
  {  
  }

  size_t Id() const {
    return id_;
  }

  void Eat() {
    while (true) {
      lfork_->Get();
      if (rfork_->TryGet()) {
        break;
      } else {
        lfork_->Put();
      }
    }
  }

  void Think() {
    lfork_->Put();
    rfork_->Put();
  }

 private:
  const size_t id_;
  Fork *lfork_, *rfork_;
};

