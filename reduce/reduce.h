#pragma once

#include <atomic>
#include <cstddef>
#include <thread>
#include<vector>

template <typename RandomAccessIterator, class T, class Func>
T parallel_reduce(RandomAccessIterator begin, RandomAccessIterator end,
                  const T& initial_value, Func func, size_t n_threads) {
  size_t len = end - begin;
  size_t block_len = (len + n_threads - 1) / n_threads;
  if (len < n_threads) { // stupid case
    n_threads = len;
    block_len = 1;
  }
  std::vector<std::thread> threads;
  std::atomic<T> global_res = initial_value;
  for (size_t i = 0; i < n_threads; ++i) {
    threads.emplace_back([&] (RandomAccessIterator piece_begin, RandomAccessIterator piece_end) {
        T local_res = initial_value;
        for (auto it = piece_begin; it != piece_end; ++it) {
            local_res = func(local_res, *it);
        }
        global_res.store(func(global_res.load(), local_res));
    }, begin + i * block_len, begin + std::min(len, (i + 1) * block_len));
  }
  for (auto& th : threads) {
    th.join();
  }
  return global_res;
}

