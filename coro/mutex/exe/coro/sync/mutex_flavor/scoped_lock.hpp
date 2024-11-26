#pragma once

#include <coroutine>

namespace exe::coro {

namespace mutex_flavor {

class ScopedLockMutex {
  using Mutex = ScopedLockMutex;

  struct Waiter;

 public:
  class LockGuard {
    friend struct Waiter;

   public:
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

    ~LockGuard() {
      mutex_->Unlock();
    }

   private:
    explicit LockGuard(Mutex* mutex)
        : mutex_(mutex) {
    }

   private:
    Mutex* mutex_;
  };

 private:
  struct [[nodiscard]] Waiter {
    explicit Waiter(Mutex* m)
        : mutex(m) {
      // Not implemented
    }

    bool await_ready() {  // NOLINT
      return false;       // Not implemented
    }

    void await_suspend(std::coroutine_handle<>) {  // NOLINT
      // Not implemented
    }

    LockGuard await_resume() {  // NOLINT
      return LockGuard{mutex};
    }

    Mutex* mutex;
  };

 public:
  // Asynchronous
  auto ScopedLock() {
    return Waiter{this};  // Not implemented
  }

 private:
  // Synchronous
  void Unlock() {
    // Not implemented
  }

 private:
  // ???
};

}  // namespace mutex_flavor

}  // namespace exe::coro
