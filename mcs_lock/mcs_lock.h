#pragma once

#include <atomic>
#include <thread>

class Node {
 public:
  Node() : acquired(false), next(nullptr) {}

 private:
  std::atomic<bool> acquired;
  std::atomic<Node*> next;

  friend class MCSLock;
};

class MCSLock {
 public:
  MCSLock() = default;

  void Lock() {
    cur_node_.next.store(nullptr);
    cur_node_.acquired.store(false);
    Node *prev_node_ = tail_.exchange(&cur_node_);
    if (prev_node_) {
      prev_node_->next.store(&cur_node_);
    } else {
      cur_node_.acquired.store(true);
    }
    while (!cur_node_.acquired.load()) {
      std::this_thread::yield();
    }
  }

  void Unlock() {
    Node* cur_node_ptr = &cur_node_;
    if (tail_.compare_exchange_weak(cur_node_ptr, nullptr)) {
      return;
    }
    while (!cur_node_.next.load()) {
      // chill
    }
    cur_node_.next.load()->acquired.store(true);
  }

 private:
  std::atomic<Node*> tail_;
  thread_local static Node cur_node_;
};

