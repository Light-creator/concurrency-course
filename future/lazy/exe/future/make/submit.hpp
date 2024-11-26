#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/thunk/stub.hpp>

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
Future<std::invoke_result_t<F>> auto Submit(
    sched::task::IScheduler& /*scheduler*/, F /*user*/) {
  return thunk::Stub<std::invoke_result_t<F>>{};
}

}  // namespace exe::future
