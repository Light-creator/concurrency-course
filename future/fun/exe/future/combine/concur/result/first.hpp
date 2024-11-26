#pragma once

#include <exe/future/type/result.hpp>

namespace exe::future {

// First success / last error

template <typename T>
TryFuture<T> FirstOk(TryFuture<T>, TryFuture<T>) {
  std::abort();  // Not implemented
}

}  // namespace exe::future
