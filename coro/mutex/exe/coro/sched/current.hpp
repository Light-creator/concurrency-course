#pragma once

#include <exe/sched/task/scheduler.hpp>

#include <coroutine>

namespace exe::coro {

namespace detail {

struct CurrentSchedulerAwaiter {
  bool await_ready() {  // NOLINT
    return false;       // Not implemented
  }

  void await_suspend(std::coroutine_handle<>) {  // NOLINT
    // Not implemented
  }

  sched::task::IScheduler* await_resume() {  // NOLINT
    return nullptr;                          // Not implemented
  }
};

}  // namespace detail

inline auto CurrentScheduler() {
  return detail::CurrentSchedulerAwaiter{};
}

}  // namespace exe::coro
