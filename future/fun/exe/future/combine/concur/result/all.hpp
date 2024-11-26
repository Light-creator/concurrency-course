#pragma once

#include <exe/future/type/result.hpp>

#include <tuple>

namespace exe::future {

template <typename A, typename B>
TryFuture<std::tuple<A, B>> BothOk(TryFuture<A>, TryFuture<B>) {
  std::abort();  // Not implemented
}

}  // namespace exe::future
