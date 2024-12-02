#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <semaphore>
#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/mutex.hpp>
#include <condition_variable>

#include <fmt/core.h>

class WaitGroup {
 public:
  void Add(int num) {
    std::unique_lock<std::mutex> lk(m_);

    if(count_.load()+num >= 0) count_.fetch_add(num);

    // count_.fetch_add(num);
    flag_.store(true);

    if(count_.load() > 0 || group_.load() == 0) return;

    while(group_.load() > 0) {
      group_.fetch_sub(1);
      cv_.notify_one();
    }

    flag_.store(false);
  }

  void Done() {
    Add(-1);
  }

  void Wait() {
    std::unique_lock<std::mutex> lk(m_);

    if(count_.load() == 0) return;

    group_.fetch_add(1);
    
    // while(count_.load() == 0) cv_.wait(lk);
    cv_.wait(lk, [this]() {
      return count_.load() == 0;
    });
  }

 private:
  std::atomic<int> count_{0};
  std::atomic<int> group_{0};
  std::atomic<bool> flag_{false};
  std::mutex m_;
  std::condition_variable cv_;

  std::mutex waiter_;
};
