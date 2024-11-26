#pragma once

#include <exe/future/type/result.hpp>

#include "../ready.hpp"

#include <exe/result/make/err.hpp>

namespace exe::future {

struct [[nodiscard]] Failure {
  Error e;

  template <typename T>
  operator TryFuture<T>() const {  // NOLINT
    return Ready<Result<T>>(result::Err(e));
  }
};

/*
 * Usage:
 *
 * future::TryFuture<int> Attempt() {
 *   return future::Err(TimeoutError());
 * }
 *
 */

inline auto Err(Error e) {
  return Failure{e};
}

}  // namespace exe::future
