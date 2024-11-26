#pragma once

#include <exe/future/type/future.hpp>

#include <cstdlib>  // std::abort

namespace exe::future {

/*
 * Ready value
 *
 * Usage:
 *
 * auto f = future::Ready(result::Ok(42));
 *
 */

template <typename T>
Future<T> Ready(T) {
  std::abort();  // Not implemented
}

}  // namespace exe::future
