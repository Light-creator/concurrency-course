#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/syntax/pipe.hpp>

#include <exe/future/trait/value_of.hpp>
#include <exe/future/trait/materialize.hpp>

#include <cstdlib>  // std::abort

namespace exe::future {

template <SomeFuture Future>
void Detach(Future) {
  std::abort();  // Not implemented
}

// Chaining

namespace pipe {

struct [[nodiscard]] Detach {
  template <SomeFuture Future>
  void Pipe(Future f) {
    future::Detach(std::move(f));
  }
};

}  // namespace pipe

inline auto Detach() {
  return pipe::Detach{};
}

}  // namespace exe::future
