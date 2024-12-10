#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>
#include "mutex.hpp"

#include <fmt/core.h>

#include "AtomicWrapper.hpp"

#include <cstdint>
#include <vector>

struct cond_node_t {
  twist::ed::std::atomic<uint32_t> flag_{0};
  cond_node_t* next_ = nullptr;
};

struct cond_list_t {
  cond_node_t* node_ = nullptr;
  Mutex m_;
};

class CondVar {
public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& m) {
    
    lst_.m_.lock();
      
    cond_node_t* new_node = new cond_node_t;
    new_node->next_ = lst_.node_;
    lst_.node_ = new_node;

    lst_.m_.unlock();

    m.unlock();
    
    if(new_node) {
      twist::ed::futex::Wait(new_node->flag_, 0);
      delete new_node;
    }
       
    m.lock();
  }

  void NotifyOne() {
    lst_.m_.lock();
    
    if(lst_.node_) {
      auto wake_key = twist::ed::futex::PrepareWake(lst_.node_->flag_);
      lst_.node_->flag_.store(1);

      lst_.node_ = lst_.node_->next_;

      twist::ed::futex::WakeOne(wake_key);
    }

    lst_.m_.unlock();
  }

  void NotifyAll() {
    lst_.m_.lock();

    while(lst_.node_) {
      auto wake_key = twist::ed::futex::PrepareWake(lst_.node_->flag_);
      lst_.node_->flag_.store(1);

      lst_.node_ = lst_.node_->next_;

      twist::ed::futex::WakeOne(wake_key);
    }

    lst_.m_.unlock();
  }

private:
  cond_list_t lst_;
};
