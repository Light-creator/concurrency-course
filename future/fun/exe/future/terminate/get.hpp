#pragma once

#include <exe/future/syntax/pipe.hpp>

#include <exe/thread/event.hpp>

#include <cstdlib>  // std::abort
#include <utility>  // std::move

namespace exe::future {

/*
 * Unwrap Future synchronously (blocking current thread)
 *
 * Usage:
 *
 * future::Get(future::Submit(pool, [] { return 7; }));
 *
 */

template <typename T>
T Get(Future<T>) {
  std::abort();  // Not implemented
}

// Chaining

namespace pipe {

struct [[nodiscard]] Get {
  template <typename T>
  T Pipe(Future<T> f) {
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
