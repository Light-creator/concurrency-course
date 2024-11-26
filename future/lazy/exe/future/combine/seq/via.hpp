#pragma once

#include <exe/sched/task/scheduler.hpp>

#include <exe/future/syntax/pipe.hpp>
#include <exe/future/trait/value_of.hpp>
#include <exe/future/thunk/stub.hpp>

namespace exe::future {

namespace pipe {

struct [[nodiscard]] Via {
  sched::task::IScheduler* scheduler;

  explicit Via(sched::task::IScheduler& s)
      : scheduler(&s) {
  }

  // Non-copyable
  Via(const Via&) = delete;

  template <SomeFuture InputFuture>
  Future<trait::ValueOf<InputFuture>> auto Pipe(InputFuture) {
    return thunk::Stub<trait::ValueOf<InputFuture>>{};
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
