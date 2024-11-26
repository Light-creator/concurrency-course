#pragma once

#include <exe/future/type/future.hpp>

#include <exe/sched/task/submit.hpp>

#include <type_traits>

namespace exe::future {

/*
 * Computation (to be) scheduled to the given task scheduler
 *
 * Usage:
 *
 * auto f = future::Submit(pool, [] {
 *   return 42;  // ~ computation
 * });
 *
 */

template <typename F>
Future<std::invoke_result_t<F>> Submit(sched::task::IScheduler& /*scheduler*/,
                                       F /*user*/) {
  std::abort();  // Not implemented
}

}  // namespace exe::future
