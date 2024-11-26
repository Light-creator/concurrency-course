#pragma once

#include <array>
#include <span>

// Single-Producer / Multi-Consumer Ring Buffer

template <typename Task, size_t Capacity>
class WorkStealingQueue {
  struct Slot {
    // ???
  };

 public:
  bool TryPush(Task* /*task*/) {
    return false;  // Not implemented
  }

  Task* TryPop() {
    return nullptr;  // Not implemented
  }

  size_t Grab(std::span<Task*> /*out_buffer*/) {
    return 0;  // Not implemented
  }

  size_t SpaceLowerBound() const {
    return 0;  // Not implemented
  }

 private:
  std::array<Slot, Capacity> buffer_;
  // ???
};
