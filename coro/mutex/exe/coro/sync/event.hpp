#pragma once

#include <coroutine>

namespace exe::coro {

class Event {
  struct [[nodiscard]] Waiter {
    explicit Waiter(Event*) {
      // Not implemented
    }

    // Awaiter

    bool await_ready() {  // NOLINT
      return false;       // Not implemented
    }

    void await_suspend(std::coroutine_handle<>) {  // NOLINT
      // Not implemented
    }

    void await_resume() {  // NOLINT
      // Not implemented
    }
  };

 public:
  // Asynchronous
  auto Wait() {
    return Waiter{this};  // Not implemented
  }

  // One-shot
  void Fire() {
    // Not implemented
  }

 private:
  // ???
};

}  // namespace exe::coro
