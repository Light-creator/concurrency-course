#pragma once

#include <exe/future/syntax/pipe.hpp>

#include <type_traits>

#include <cstdlib>  // std::abort
#include <type_traits>
#include <utility>  // std::move

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

  template <typename T>
  using U = std::invoke_result_t<F, T>;

  template <typename T>
  Future<U<T>> Pipe(Future<T>) {
    std::abort();  // Not implemented
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
