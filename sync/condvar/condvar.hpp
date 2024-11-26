#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

#include "AtomicWrapper.hpp"

#include <cstdint>
#include <vector>

class CondVar {
public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& m) {
    // m.lock();
    // if(holder_idx == -1) holder_idx = v_locked.size();
    // v_locked.push_back(AtomicWrapper<uint32_t>(0));
    
    // uint32_t expected = 0;
    // int idx = holder_idx;
    // while(v_locked[idx].val.compare_exchange_weak(
    //   expected,
    //   1,
    //   std::memory_order_acquire
    // )) {
    //   expected = 0;
    //   twist::ed::futex::Wait(v_locked[idx].val, 0);
    // }
    locked_.store(1);
    m.unlock();
    twist::ed::futex::Wait(locked_, 1);
    locked_.fetch_sub(1, std::memory_order_relaxed);
    m.lock();
    // while(locked_.compare_exchange_weak(
    //   expected,
    //   1,
    //   std::memory_order_acquire
    // )) {
    //   expected = 0;
    // }
    // 
    // m.unlock();
  }

  void NotifyOne() {
    auto wake_key = twist::ed::futex::PrepareWake(locked_);
    locked_.store(0);
    twist::ed::futex::WakeAll(wake_key);
    // auto wake_key = twist::ed::futex::PrepareWake(v_locked[holder_idx].val);
    // v_locked[holder_idx].val.store(0, std::memory_order_release);
    // if((int)v_locked.size() > holder_idx+1) holder_idx++;
    // else holder_idx = -1;
    // twist::ed::futex::WakeAll(wake_key);
  }

  void NotifyAll() {
    while(true) {
      NotifyOne();
    } 
  }

private:
  // std::vector<AtomicWrapper<uint32_t>> v_locked;
  // int holder_idx = -1;
  twist::ed::std::atomic<uint32_t> locked_;
  // twist::ed::std::atomic<uint32_t> count = 0;
  // twist::ed::std::atomic<bool> *q;
};
