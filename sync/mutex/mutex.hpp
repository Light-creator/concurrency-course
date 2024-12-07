#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

#include <cstdint>
#include <fmt/core.h>

class Mutex {
 public: 
  uint32_t cmpxchg(uint32_t expected, uint32_t desired) {
    uint32_t* old = &expected;
    locked_.compare_exchange_strong(expected, desired);
    return *old;
  }

  void Lock() {
    uint32_t state = cmpxchg(0, 1);
    
    // state != 0 -> we have to wait
    if(state != 0) {
      do {
        // if we already have waiters || waiters are comming
        if(state == 2 || cmpxchg(1, 2)) {
          twist::ed::futex::Wait(locked_, 2);
        }

        // try to lock with waiters until we will be waken up
      } while((state = cmpxchg(0, 2)) != 0);
    }
  }

  void Unlock() {
    if(locked_.fetch_sub(1) != 1) {
      auto wake_key = twist::ed::futex::PrepareWake(locked_);
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
  // 0 - unloked
  // 1 - loked without waiters
  // 2 - loked with waiters
  twist::ed::std::atomic<uint32_t> locked_{0};
};
