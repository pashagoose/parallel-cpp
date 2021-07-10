#pragma once

#include <atomic>
#include <thread>

#include <iostream>

class Node {
 public:
  Node() : acquired(true), next(nullptr) {}

 private:
  std::atomic<bool> acquired;
  std::atomic<Node*> next;

  friend class MCSLock;
};

class MCSLock {
 public:
  MCSLock() = default;

  void Lock() {
    node_.next.store(nullptr);
    node_.acquired.store(true);
    Node *prev_node_ = tail_.exchange(&node_);
    if (prev_node_) {
      prev_node_->next.store(&node_);
      node_.acquired.store(false);
    }
    while (!node_.acquired.load()) {
      std::this_thread::yield();
    }
  }

  void Unlock() {
    Node* node_ptr = &node_;
    if (tail_.compare_exchange_weak(node_ptr, nullptr)) {
      return;
    }
    while (!node_.next.load()) {
      // chill
    }
    node_.next.load()->acquired.store(true);
  }

 private:
  std::atomic<Node*> tail_{nullptr};
  thread_local static inline Node node_ = Node();
};
