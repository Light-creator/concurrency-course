#pragma once

#include "shared_state.hpp"

#include <cassert>

namespace exe::future {

template <typename T>
class Future {
 public:
  using ValueType = T;

  // Future

  // Move-constructible
  Future(Future&&) {
    // Not implemented
  }

  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Non-move-assignable
  Future& operator=(Future&&) = delete;

  ~Future() {
    // Not implemented
  }

  // One-shot
  void Consume(Callback<T>) && {
    // Not implemented
  }

 private:
  // SharedState<T>*
};

}  // namespace exe::future
