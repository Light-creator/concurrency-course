#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/thunk/stub.hpp>

#include <cassert>
#include <tuple>

namespace exe::future {

// Producer

template <typename T>
class Promise {
 public:
  Promise(/*?*/) {
    // Not implemented
  }

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Move-constructible
  Promise(Promise&&) {
    // Not implemented
  }

  // Non-move-assignable
  Promise& operator=(Promise&&) = delete;

  ~Promise() {
    // Not implemented
  }

  // One-shot
  void Set(T) && {
    // Not implemented
  }
};

/*
 * Asynchronous one-shot contract
 *
 * Usage:
 *
 * auto [f, p] = future::Contract<int>();
 *
 * // Producer
 * std::move(p).Set(7);
 *
 * // Consumer
 * auto v = future::Get(std::move(f));  // 7
 *
 */

template <typename T>
std::tuple<thunk::Stub<T>, Promise<T>> Contract() {
  return {thunk::Stub<T>{}, Promise<T>{}};
}

}  // namespace exe::future
