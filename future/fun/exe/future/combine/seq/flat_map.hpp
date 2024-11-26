#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/trait/value_of.hpp>

#include <cstdlib>  // std::abort
#include <type_traits>
#include <utility>  // std::move

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

  template <typename T>
  using U = future::trait::ValueOf<std::invoke_result_t<F, T>>;

  template <typename T>
  Future<U<T>> Pipe(Future<T>) {
    std::abort();  // Not implemented
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
