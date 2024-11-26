#pragma once

#include "shared_state.hpp"

#include <cassert>

namespace exe::future {

template <typename T>
class Promise {
 public:
  // Move-constructible
  Promise(Promise&&) {
    // Not implemented
  }

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Non-move-assignable
  Promise& operator=(Promise&&) = delete;

  ~Promise() {
    // Not implemented
  }

  // One-shot
  void Set(T) && {
    // Not implemented
  }

 private:
  // SharedState<T>*
};

}  // namespace exe::future
