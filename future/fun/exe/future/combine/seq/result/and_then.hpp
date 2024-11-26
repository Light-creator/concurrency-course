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
struct [[nodiscard]] AndThen {
  F user;

  explicit AndThen(F u)
      : user(std::move(u)) {
  }

  // Non-copyable
  AndThen(const AndThen&) = delete;

  template <typename T>
  using U = result::trait::ValueOf<
      future::trait::ValueOf<std::invoke_result_t<F, T>>>;

  template <typename T>
  TryFuture<U<T>> Pipe(TryFuture<T>) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

/*
 * Asynchronous try-catch
 *
 * TryFuture<T> -> (T -> TryFuture<U>) -> TryFuture<U>
 *
 */

template <typename F>
auto AndThen(F user) {
  return pipe::AndThen{std::move(user)};
}

}  // namespace exe::future
