#pragma once

#include <exe/future/type/result.hpp>

#include <cstdlib>  // std::abort
#include <type_traits>
#include <utility>  // std::move

namespace exe::future {

namespace pipe {

template <typename F>
struct [[nodiscard]] MapOk {
  F user;

  explicit MapOk(F u)
      : user(std::move(u)) {
  }

  // Non-copyable
  MapOk(const MapOk&) = delete;

  template <typename T>
  using U = std::invoke_result_t<F, T>;

  template <typename T>
  TryFuture<U<T>> Pipe(TryFuture<T>) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

// TryFuture<T> -> (T -> U) -> TryFuture<U>

template <typename F>
auto MapOk(F user) {
  return pipe::MapOk(std::move(user));
}

}  // namespace exe::future
