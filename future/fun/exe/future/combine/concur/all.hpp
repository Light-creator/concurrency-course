#pragma once

#include <exe/future/type/future.hpp>

#include <tuple>

namespace exe::future {

template <typename A, typename B>
Future<std::tuple<A, B>> Both(Future<A>, Future<B>) {
  std::abort();  // Not implemented
}

}  // namespace exe::future
