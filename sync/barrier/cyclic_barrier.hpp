#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>
#include <fmt/core.h>

#include <cstddef>

class CyclicBarrier {
public:
  explicit CyclicBarrier(size_t c): cap_(c) {}

  void ArriveAndWait() {
    std::unique_lock<twist::ed::std::mutex> lk(m_);
    
    int my_group = group_;
    if(++sz_ < cap_) {
      while(group_ == my_group) cv_.wait(lk);
    } else {
      sz_ = 0;
      group_++;
      cv_.notify_all();
    }
  } 

private: 
  size_t cap_;
  size_t sz_ = 0;
  int group_ = 0;
  
  twist::ed::std::mutex m_;
  twist::ed::std::condition_variable cv_;
};
