#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>

#include <optional>

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  void Push(T) {
    // Not implemented
  }

  std::optional<T> Pop() {
    return std::nullopt;  // Not implemented
  }

  void Close() {
    // Not implemented
  }

 private:
  // Buffer
};
