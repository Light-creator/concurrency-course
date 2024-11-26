#pragma once

#include "tagged_semaphore.hpp"

#include <deque>
#include <memory>
#include <fmt/core.h>

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(size_t cap): cap_(cap), sema_put_(cap), sema_take_(cap), sema_(1) {
    for(size_t i=0; i<cap_; i++) {
      // if(i < cap_-1) q_put_.emplace_back(std::move(sema_put_.Acquire()));
      q_take_.emplace_back(std::move(sema_take_.Acquire()));
    }
  }

  ~BoundedBlockingQueue() {
    while(!q_put_.empty()) {
      sema_put_.Release(std::move(q_put_.front()));
      q_put_.pop_front();
    }

    while(!q_take_.empty()) {
      sema_take_.Release(std::move(q_take_.front()));
      q_take_.pop_front();
    }
  }

  void Put(T item) {
    auto tk = sema_put_.Acquire();

    auto local_tk = sema_.Acquire();

    q_put_.emplace_back(std::move(tk));

    q_.push_back(std::move(item));

    sema_take_.Release(std::move(q_take_.front()));
    q_take_.pop_front();

    sema_.Release(std::move(local_tk));
  }

  T Take() {
    auto tk = sema_take_.Acquire();
    
    auto local_tk = sema_.Acquire();
    
    q_take_.emplace_back(std::move(tk));

    T item{std::move(q_.front())};
    q_.pop_front();

    sema_put_.Release(std::move(q_put_.front()));
    q_put_.pop_front();
    
    sema_.Release(std::move(local_tk));
    return item;
  }

 private:
  // Tags
  struct SomeTag {};

 private:
  // std::unique_ptr<class TaggedSemaphore<SomeTag>::Token> tk_;
  std::deque<T> q_;

  std::deque<class TaggedSemaphore<SomeTag>::Token> q_put_;
  std::deque<class TaggedSemaphore<SomeTag>::Token> q_take_;

  size_t cap_;

  TaggedSemaphore<SomeTag> sema_put_;
  TaggedSemaphore<SomeTag> sema_take_;

  TaggedSemaphore<SomeTag> sema_;
};


