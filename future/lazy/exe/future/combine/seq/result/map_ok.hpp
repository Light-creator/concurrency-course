#pragma once

#include <exe/future/type/result.hpp>
#include <exe/future/syntax/pipe.hpp>
#include <exe/future/trait/value_of.hpp>
#include <exe/future/thunk/stub.hpp>

#include <exe/result/trait/value_of.hpp>

#include <type_traits>

namespace exe::future {

namespace pipe {

template <typename F>
struct [[nodiscard]] MapOk {
  F user;

  explicit MapOk(F&& u)
      : user(std::move(u)) {
  }

  // Non-copyable
  MapOk(const MapOk&) = delete;

  template <SomeTryFuture InputFuture>
  SomeTryFuture auto Pipe(InputFuture) {
    using ResultT = trait::ValueOf<InputFuture>;
    using T = result::trait::ValueOf<ResultT>;
    using U = std::invoke_result_t<F, T>;

    return thunk::Stub<Result<U>>{};
  }
};

}  // namespace pipe

// TryFuture<T> -> (T -> U) -> TryFuture<U>

template <typename F>
auto MapOk(F user) {
  return pipe::MapOk(std::move(user));
}

}  // namespace exe::future
