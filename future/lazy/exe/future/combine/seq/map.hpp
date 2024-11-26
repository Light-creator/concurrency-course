#pragma once

#include <exe/future/syntax/pipe.hpp>
#include <exe/future/trait/value_of.hpp>
#include <exe/future/thunk/stub.hpp>

namespace exe::future {

namespace pipe {

template <typename F>
struct [[nodiscard]] Map {
  F user;

  explicit Map(F u)
      : user(std::move(u)) {
  }

  // Non-copyable
  Map(const Map&) = delete;

  template <SomeFuture InputFuture>
  SomeFuture auto Pipe(InputFuture) {
    using T = trait::ValueOf<InputFuture>;
    using U = std::invoke_result_t<F, T>;

    return thunk::Stub<U>{};
  }
};

}  // namespace pipe

/*
 * Functor
 * https://wiki.haskell.org/Typeclassopedia
 *
 * Future<T> -> (T -> U) -> Future<U>
 *
 */

template <typename F>
auto Map(F user) {
  return pipe::Map{std::move(user)};
}

}  // namespace exe::future
