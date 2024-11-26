#pragma once

#include <coroutine>

namespace exe::coro {

namespace detail {

struct YieldAwaiter {
  YieldAwaiter() = default;

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

}  // namespace detail

inline auto Yield() {
  return detail::YieldAwaiter{};
}

}  // namespace exe::coro
