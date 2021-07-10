#pragma once

#include <atomic>
#include <optional>

template <typename T>
class MPSCQueue {
 public:
  MPSCQueue() = default;

  ~MPSCQueue() {
    auto current_head = head.load();
    while (current_head) {
      head.store(current_head->next);
      delete current_head;
      current_head = head.load();
    }
  }

  void Push(const T& value) {
    Node* ptr_node = new Node(value);
    for (;;) {
      ptr_node->next.store(head.load());
      Node* cpy = ptr_node->next.load();
      if (head.compare_exchange_weak(cpy, ptr_node)) {
        return;
      }
    }
  }

  std::optional<T> Pop() {
    auto current_head = head.load();
    for (;;) {
      if (!current_head) {
        return std::nullopt;
      }
      if (head.compare_exchange_weak(current_head, current_head->next.load())) {
        break;
      }
      current_head = head.load();
    }
    std::optional<T> result(current_head->element.load());
    delete current_head;
    return result;
  }

 private:
  
  struct Node {
    std::atomic<Node*> next;
    std::atomic<T> element;

    Node() : next(nullptr) {}

    Node(const T& value) : next(nullptr), element(value) {}
  };

  std::atomic<Node*> head{nullptr};
};

