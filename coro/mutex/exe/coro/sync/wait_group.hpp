#pragma once

#include <coroutine>

namespace exe::coro {

class WaitGroup {
  struct [[nodiscard]] Waiter {
    explicit Waiter(WaitGroup*) {
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
  void Add(size_t /*count*/) {
    // Not implemented
  }

  void Done() {
    // Not implemented
  }

  // Asynchronous
  auto Wait() {
    return Waiter{this};  // Not implemented
  }

 private:
  // ???
};

}  // namespace exe::coro
