#pragma once

#include <atomic>
#include <thread>

class Node {
 private:
  std::atomic<bool> acquired;
  std::atomic<Node*> next;

  friend class MCSLock;
};

class MCSLock {
 public:
  MCSLock() {
  }

  void Lock() {
    // Your code
  }

  void Unlock() {
    // Your code
  }

 private:
  std::atomic<Node*> tail_;
  thread_local std::atomic<Node*> cur_node_;
};

