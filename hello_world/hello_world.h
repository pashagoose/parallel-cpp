#pragma once

#include<mutex>
#include <ostream>
#include <string_view>
#include<thread>
#include<vector>

class HelloWorld {
 public:
  static constexpr std::string_view kHelloPrefix = "Hello, World! From thread ";

  HelloWorld(size_t n_threads) :
                    n_threads_(n_threads) 
  {
    
  }

  void SayHello(std::ostream& os) {
      std::vector<std::thread> threads;
      for (size_t i = 0; i < n_threads_; ++i) {
        threads.emplace_back([&] {
          control_output.lock();
          os << kHelloPrefix << std::this_thread::get_id() << '\n';
          control_output.unlock();
        });
      }
      for (auto& th : threads) {
        th.join();
      }
  }

 private:
  size_t n_threads_;
  std::mutex control_output;
};
