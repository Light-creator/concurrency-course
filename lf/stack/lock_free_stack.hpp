#pragma once

#include "stamped_ptr.hpp"

#include <twist/ed/std/atomic.hpp>

#include <twist/assist/memory.hpp>

#include <optional>

// Treiber unbounded lock-free stack
// https://en.wikipedia.org/wiki/Treiber_stack

template <typename T>
class LockFreeStack {
  struct Node {
    T value;
    // ???
  };

 public:
  void Push(T /*value*/) {
    // Not implemented
  }

  std::optional<T> TryPop() {
    return std::nullopt;  // Not implemented
  }

  ~LockFreeStack() {
    // Not implemented
  }

 private:
  // ???
};
