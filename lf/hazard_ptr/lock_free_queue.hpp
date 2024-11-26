#pragma once

#include "memory/hazard/manager.hpp"

#include <twist/ed/std/atomic.hpp>

#include <twist/assist/memory.hpp>
#include <twist/trace/scope.hpp>

#include <cstdlib>  // std::abort
#include <optional>

// Michael-Scott unbounded lock-free queue
// https://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf

template <typename T>
class LockFreeQueue {
  struct Node {
    std::optional<T> value;
    // ???
  };

 public:
  LockFreeQueue() {
    // Not implemented
  }

  void Push(T /*value*/) {
    // Not implemented
  }

  std::optional<T> TryPop() {
    std::abort();  // Not implemented
  }

  ~LockFreeQueue() {
    // Not implemented
  }

 private:
  // ???
};
