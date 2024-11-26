#pragma once

#include <exe/sched/task/scheduler.hpp>

#include <exe/future/syntax/pipe.hpp>

#include <cstdlib>  // std::abort

namespace exe::future {

namespace pipe {

struct [[nodiscard]] Via {
  sched::task::IScheduler* scheduler;

  explicit Via(sched::task::IScheduler& s)
      : scheduler(&s) {
  }

  // Non-copyable
  Via(const Via&) = delete;

  template <typename T>
  Future<T> Pipe(Future<T>) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

/*
 * Scheduling state
 *
 * Future<T> -> Scheduler -> Future<T>
 *
 */

inline auto Via(sched::task::IScheduler& scheduler) {
  return pipe::Via{scheduler};
}

}  // namespace exe::future
