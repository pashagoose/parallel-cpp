#pragma once

#include <cstddef>
#include <numeric>
#include <thread>
#include <vector>

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
  std::vector<T> results(n_threads, initial_value);
  for (size_t i = 0; i < n_threads; ++i) {
    threads.emplace_back([&] (RandomAccessIterator piece_begin, RandomAccessIterator piece_end, size_t ind_res) {
        T local_res = initial_value;
        for (auto it = piece_begin; it != piece_end; ++it) {
            local_res = func(local_res, *it);
        }
        results[ind_res] = local_res;
    }, begin + i * block_len, begin + std::min(len, (i + 1) * block_len), i);
  }
  for (auto& th : threads) {
    th.join();
  }
  return std::reduce(results.begin(), results.end(), initial_value, func);
}

