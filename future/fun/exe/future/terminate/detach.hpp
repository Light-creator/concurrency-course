#pragma once

#include <exe/future/syntax/pipe.hpp>

#include <utility>  // std::move

namespace exe::future {

// Consume Future, ignore value

template <typename T>
void Detach(Future<T>) {
  // Not implemented
}

// Chaining

namespace pipe {

struct [[nodiscard]] Detach {
  template <typename T>
  void Pipe(Future<T> f) {
    future::Detach(std::move(f));
  }
};

}  // namespace pipe

inline auto Detach() {
  return pipe::Detach{};
}

}  // namespace exe::future
