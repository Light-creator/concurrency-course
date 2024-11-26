#pragma once

#include <exe/future/model/thunk.hpp>

namespace exe::future {

namespace thunk {

template <typename V>
struct Box {
  template <Thunk Producer>
  explicit Box(Producer&&) {
    // Not implemented
  }

  using ValueType = V;

  // Computation
  struct Abort {
    void Start() {
      std::abort();
    }
  };

  template <Continuation<V> Consumer>
  Computation auto Materialize(Consumer&&) {
    return Abort{};  // Not implemented
  }
};

}  // namespace thunk

}  // namespace exe::future
