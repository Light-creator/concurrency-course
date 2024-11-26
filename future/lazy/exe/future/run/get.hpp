#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/syntax/pipe.hpp>

#include <exe/future/trait/value_of.hpp>
#include <exe/future/trait/materialize.hpp>

#include <cstdlib>  // std::abort

namespace exe::future {

/*
 * Unwrap Future synchronously (blocking current thread)
 *
 * Usage:
 *
 * future::Get(future::Submit(pool, [] { return 7; }));
 *
 */

template <SomeFuture Future>
trait::ValueOf<Future> Get(Future) {
  std::abort();  // Not implemented
}

// Chaining

namespace pipe {

struct [[nodiscard]] Get {
  template <SomeFuture Future>
  trait::ValueOf<Future> Pipe(Future f) {
    return future::Get(std::move(f));
  }
};

}  // namespace pipe

/*
 * Usage:
 *
 * auto v = future::Submit(pool, { return 7; }) | future::Get();
 */

inline auto Get() {
  return pipe::Get{};
}

}  // namespace exe::future
