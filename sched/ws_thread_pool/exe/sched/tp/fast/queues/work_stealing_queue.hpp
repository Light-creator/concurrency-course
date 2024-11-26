#pragma once

#include <exe/sched/task/task.hpp>

#include <twist/ed/std/atomic.hpp>

#include <twist/assist/preempt.hpp>

#include <cassert>
#include <array>
#include <span>

namespace exe::sched::tp::fast {

// SP/MC lock-free ring buffer for local tasks

template <size_t Capacity>
class WorkStealingQueue {
  struct Slot {
    // ???
  };

 public:
  // Producer

  bool TryPush(task::TaskBase* /*task*/) {
    return false;  // Not implemented;
  }

  // Consumers

  task::TaskBase* TryPop() {
    return nullptr;  // Not implemented
  }

  size_t Grab(std::span<task::TaskBase*> /*out_buffer*/) {
    return 0;  // Not implemented
  }

 private:
  std::array<Slot, Capacity> buffer_;
};

}  // namespace exe::sched::tp::fast
