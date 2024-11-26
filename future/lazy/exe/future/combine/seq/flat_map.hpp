#pragma once

#include <exe/future/syntax/pipe.hpp>
#include <exe/future/trait/value_of.hpp>
#include <exe/future/thunk/stub.hpp>

#include <type_traits>

namespace exe::future {

namespace pipe {

template <typename F>
struct [[nodiscard]] FlatMap {
  F user;

  explicit FlatMap(F u)
      : user(std::move(u)) {
  }

  // Non-copyable
  FlatMap(const FlatMap&) = delete;

  template <SomeFuture InputFuture>
  SomeFuture auto Pipe(InputFuture) {
    using T = trait::ValueOf<InputFuture>;
    using FutureU = std::invoke_result_t<F, T>;
    using U = trait::ValueOf<FutureU>;

    return thunk::Stub<U>{};
  }
};

}  // namespace pipe

/*
 * Compose two asynchronous actions sequentially
 *
 * Monadic bind
 * https://wiki.haskell.org/Typeclassopedia
 *
 * Future<T> -> (T -> Future<U>) -> Future<U>
 *
 */

template <typename F>
auto FlatMap(F user) {
  return pipe::FlatMap{std::move(user)};
}

}  // namespace exe::future
