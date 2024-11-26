#pragma once

#include <coroutine>

namespace exe::coro {

namespace mutex_flavor {

class RunMutex {
  using Mutex = RunMutex;

  template <typename F>
  struct [[nodiscard]] Waiter {
    explicit Waiter(Mutex*, F) {
      // Not implemented
    }

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
  template <typename F>
  auto Run(F cs) {
    return Waiter<F>{this, std::move(cs)};  // Not implemented
  }

 private:
  // ???
};

}  // namespace mutex_flavor

}  // namespace exe::coro
