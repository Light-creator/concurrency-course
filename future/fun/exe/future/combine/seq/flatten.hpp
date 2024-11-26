#pragma once

#include <exe/future/syntax/pipe.hpp>

#include <cstdlib>  // std::abort

namespace exe::future {

namespace pipe {

struct [[nodiscard]] Flatten {
  Flatten() = default;

  // Non-copyable
  Flatten(const Flatten&) = delete;

  template <typename T>
  Future<T> Pipe(Future<Future<T>>) {
    std::abort();  // Not implemented
  }
};

}  // namespace pipe

/*
 * Collapse nested Future-s
 *
 * Future<Future<T>> -> Future<T>
 *
 */

inline auto Flatten() {
  return pipe::Flatten{};
}

}  // namespace exe::future
