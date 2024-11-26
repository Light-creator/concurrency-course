#pragma once

#include <exe/sched/task/task.hpp>

#include <exe/thread/spinlock.hpp>

#include <wheels/intrusive/forward_list.hpp>

#include <span>

namespace exe::sched::tp::fast {

// Unbounded task queue shared between workers

class GlobalQueue {
  using List = wheels::IntrusiveForwardList<task::TaskBase>;

  // using Mutex = twist::ed::std::mutex;
  using Mutex = exe::thread::SpinLock;

 public:
  void PushOne(task::TaskBase* /*task*/) {
    // Not implemented
  }

  void PushMany(std::span<task::TaskBase*> /*buffer*/) {
    // Not implemented
  }

  task::TaskBase* TryPopOne() {
    return nullptr;  // Not implemented
  }

  // ???
  size_t Grab(std::span<task::TaskBase*> /*out_buffer*/) {
    return 0;  // Not implemented
  }

 private:
  // ???
};

}  // namespace exe::sched::tp::fast
