#pragma once

#include <exe/future/model/thunk.hpp>

namespace exe::future {

namespace thunk {

template <typename V>
struct [[nodiscard]] Stub {
  Stub() = default;

  // Non-copyable
  Stub(const Stub&) = delete;
  Stub& operator=(const Stub&) = delete;

  Stub(Stub&&) = default;

  using ValueType = V;

  // Computation
  struct Abort {
    void Start() {
      std::abort();
    }
  };

  // Thunk
  template <Continuation<V> Consumer>
  Computation auto Materialize(Consumer&&) {
    return Abort{};
  }
};

}  // namespace thunk

}  // namespace exe::future
