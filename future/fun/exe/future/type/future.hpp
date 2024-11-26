#pragma once

#include <cassert>

namespace exe::future {

// Represents future value of type T

template <typename T>
class [[nodiscard]] Future {
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

  // ???

 private:
  // ???
};

}  // namespace exe::future
