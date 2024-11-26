#pragma once

#include <exe/fiber/sched/suspend.hpp>

namespace exe::fiber {

class Mutex {
 public:
  void Lock() {
    // Not implemented
  }

  bool TryLock() {
    return false;  // Not implemented
  }

  void Unlock() {
    // Not implemented
  }

  // Lockable

  void lock() {  // NOLINT
    Lock();
  }

  bool try_lock() {  // NOLINT
    return TryLock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  //
};

}  // namespace exe::fiber
