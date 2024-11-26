#pragma once

#include <exe/future/type/future.hpp>

namespace exe::future {

template <typename T>
Future<T> First(Future<T>, Future<T>) {
  std::abort();  // Not implemented
}

}  // namespace exe::future
