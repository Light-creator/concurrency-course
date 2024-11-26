#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>
#include <twist/ed/fmt/print.hpp>

#include "AtomicWrapper.hpp"

#include <vector>

class Mutex {
public:
  Mutex(): locked_(0) {}

  void Lock() {
    uint32_t expected = 0;
    while(!locked_.compare_exchange_weak(
      expected,
      1,
      std::memory_order_acquire
    )) {
      expected = 0;
      // twist::ed::futex::Wait(locked_, 0);
    }
  }

  void Unlock() {
    // auto wake_key = twist::ed::futex::PrepareWake(locked_);
    locked_.store(0, std::memory_order_release);
    // twist::ed::futex::WakeAll(wake_key);
  }

  void unlock() { Unlock(); }
  void lock() { Lock(); }

private:
  twist::ed::std::atomic<uint32_t> locked_;
};
// class Mutex {
// public:
//   Mutex() {}

//   void Lock() {
//     bool expected = false;

//     int idx = v_locked.size();
//     v_locked.push_back(AtomicWrapper<bool>(false));

//     for(size_t i=0; i<v_locked.size(); i++) twist::ed::fmt::println("{}", v_locked[i].val.load());


//     twist::ed::fmt::println("lock: {}", idx);
//     
//     if(holder_idx == -1) {
//       holder_idx = idx;
//     }

//     twist::ed::fmt::println("holder_idx: {}", holder_idx);

//     while(!v_locked[idx].val.compare_exchange_weak(
//       expected, 
//       true, 
//       std::memory_order_acquire
//     )) {
//       expected = false;
//     }
//   }

//   void Unlock() {
//     twist::ed::fmt::println("unlock: {}", holder_idx);
//     v_locked[holder_idx].val.store(false, std::memory_order_release);
//     if((int)v_locked.size() > holder_idx+1) {
//       holder_idx++;
//     } else {
//       holder_idx = -1;
//     }
//   }

//   // BasicLockable
//   // https://en.cppreference.com/w/cpp/named_req/BasicLockable

//   void lock() {  // NOLINT
//     Lock();
//   }

//   void unlock() {  // NOLINT
//     Unlock();
//   }

// private:
//   int holder_idx = -1;
//   std::vector<AtomicWrapper<bool>> v_locked;
// };
