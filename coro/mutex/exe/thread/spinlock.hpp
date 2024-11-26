#pragma once

namespace exe::thread {

class SpinLock {
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

}  // namespace exe::thread
