#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/state/contract.hpp>

#include <cassert>
#include <cstdlib>  // std::abort
#include <tuple>

namespace exe::future {

// Producer

template <typename T>
class Promise {
 public:
  // Promise

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
  //
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
std::tuple<Future<T>, Promise<T>> Contract() {
  std::abort();  // Not implemented
}

}  // namespace exe::future
