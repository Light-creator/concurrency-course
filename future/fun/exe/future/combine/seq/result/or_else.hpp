#pragma once

#include <exe/future/type/result.hpp>

#include <exe/result/trait/value_of.hpp>
#include <exe/future/trait/value_of.hpp>

#include <cstdlib>  // std::abort
#include <type_traits>
#include <utility>  // std::move

namespace exe::future {

namespace pipe {

template <typename F>
struct [[nodiscard]] OrElse {
  F user;

  explicit OrElse(F u)
      : user(std::move(u)) {
  }

  // Non-copyable
  OrElse(const OrElse&) = delete;

  template <typename T>
  using U = result::trait::ValueOf<
      future::trait::ValueOf<std::invoke_result_t<F, Error>>>;

  template <typename T>
  TryFuture<T> Pipe(TryFuture<T>) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

/*
 * Asynchronous try-catch
 *
 * TryFuture<T> -> (Error -> TryFuture<T>) -> TryFuture<T>
 *
 */

template <typename F>
auto OrElse(F user) {
  return pipe::OrElse{std::move(user)};
}

}  // namespace exe::future
