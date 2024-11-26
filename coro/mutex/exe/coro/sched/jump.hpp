#pragma once

#include <exe/sched/task/scheduler.hpp>

#include <coroutine>

namespace exe::coro {

namespace detail {

struct [[nodiscard]] JumpToAwaiter {
  explicit JumpToAwaiter(sched::task::IScheduler&) {
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

}  // namespace detail

inline auto JumpTo(sched::task::IScheduler& target) {
  return detail::JumpToAwaiter{target};
}

}  // namespace exe::coro
