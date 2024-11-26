#pragma once

#include <mutex>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>

#include <cstdlib>

class Semaphore {
 public:
  explicit Semaphore(size_t tokens): tokens_(tokens) {}

  void Acquire() {
    std::unique_lock<twist::ed::std::mutex> locked(m_);
    cv_.wait(locked, [this]() {
      return tokens_ > 0;
    });

    tokens_--;
  }

  void Release() {
    std::lock_guard<twist::ed::std::mutex> guard(m_);
    tokens_++;
    cv_.notify_all();
  }

 private:
  twist::ed::std::mutex m_;
  twist::ed::std::condition_variable cv_;
  size_t tokens_;
};
