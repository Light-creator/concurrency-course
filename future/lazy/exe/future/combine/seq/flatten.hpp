#pragma once

#include <exe/future/syntax/pipe.hpp>
#include <exe/future/trait/value_of.hpp>
#include <exe/future/thunk/stub.hpp>

namespace exe::future {

namespace pipe {

struct [[nodiscard]] Flatten {
  Flatten() = default;

  // Non-copyable
  Flatten(const Flatten&) = delete;

  template <SomeFuture InputFuture>
  SomeFuture auto Pipe(InputFuture) {
    using FutureT = trait::ValueOf<InputFuture>;
    using T = trait::ValueOf<FutureT>;

    return thunk::Stub<T>{};
  }
};

}  // namespace pipe

/*
 * Collapse nested Future-s
 *
 * Future<Future<T>> -> Future<T>
 *
 */

inline auto Flatten() {
  return pipe::Flatten{};
}

}  // namespace exe::future
