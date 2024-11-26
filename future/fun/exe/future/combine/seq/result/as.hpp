#pragma once

#include <exe/future/type/result.hpp>

#include <exe/result/trait/is_result.hpp>
#include <exe/result/make/ok.hpp>

#include <exe/future/combine/seq/map.hpp>

#include <type_traits>

namespace exe::future {

// Explicit

/*
 * Future<T> -> TryFuture<T'> where
 *    T' = U for T = Result<U>
 *    T' = T otherwise
 */

template <typename T>
TryFuture<T> AsTryFuture(TryFuture<T> f) {
  return std::move(f);  // Identity
}

template <typename T,
          std::enable_if_t<!result::trait::IsResult<T>, bool> = true>
TryFuture<T> AsTryFuture(Future<T> f) {
  return std::move(f) | future::Map([](T v) {
           return result::Ok(v);
         });
}

// Chaining

namespace pipe {

struct [[nodiscard]] AsTryFuture {
  AsTryFuture() = default;

  // Non-copyable
  AsTryFuture(const AsTryFuture&) = delete;

  template <typename T>
  auto Pipe(Future<T> f) {
    return future::AsTryFuture(std::move(f));
  }
};

}  // namespace pipe

auto AsTryFuture() {
  return pipe::AsTryFuture{};
}

}  // namespace exe::future
