#pragma once

#include "future.hpp"

#include <exe/result/type/result.hpp>

namespace exe::future {

// Fallible Future

template <typename T>
using TryFuture = Future<Result<T>>;

}  // namespace exe::future
