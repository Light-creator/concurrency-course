#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

#include <cstdint>
#include <fmt/core.h>

class Mutex {
 public: 
  uint32_t cmpxchg(uint32_t expected, uint32_t desired) {
    uint32_t* old = &expected;
    locked_.compare_exchange_weak(expected, desired);
    return *old;
  }

  void Lock() {
    int state = cmpxchg(0, 1);

    if(state != 0) {
      do {
        if(state == 2 || cmpxchg(1, 2) != 0) {
          twist::ed::futex::Wait(locked_, 2);
        }
      } while((state = cmpxchg(0, 2)) != 0);
    }
  }

  void Unlock() {
    auto wake_key = twist::ed::futex::PrepareWake(locked_);
    if(locked_.fetch_sub(1) != 1) {
      locked_.store(0);
      twist::ed::futex::WakeAll(wake_key);
    }
  }
  
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable

  void lock() {  // NOLINT
    Lock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  twist::ed::std::atomic<uint32_t> locked_{0};
  twist::ed::std::atomic<uint32_t> owner_{0};
};
