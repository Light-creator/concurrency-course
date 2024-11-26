#pragma once

#include <exe/future/type/result.hpp>
#include <exe/future/syntax/pipe.hpp>
#include <exe/future/trait/value_of.hpp>
#include <exe/future/thunk/stub.hpp>

#include <exe/result/trait/value_of.hpp>

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

  template <SomeTryFuture InputFuture>
  SomeTryFuture auto Pipe(InputFuture) {
    using ResultT = trait::ValueOf<InputFuture>;
    using T = result::trait::ValueOf<ResultT>;
    using FutureU = std::invoke_result_t<F, T>;
    using ResultU = trait::ValueOf<FutureU>;
    using U = result::trait::ValueOf<ResultU>;

    return thunk::Stub<Result<U>>{};
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
