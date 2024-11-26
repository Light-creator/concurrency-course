#pragma once

#include "ready.hpp"

#include <exe/result/trait/is_result.hpp>

#include <cassert>
#include <utility>  // std::move

namespace exe::future {

/*
 * Ready (plain) value
 *
 * Usage:
 *
 * auto f = future::Value(7);
 *
 */

template <typename T>
Future<T> Value(T v) {
  static_assert(!result::trait::IsResult<T>, "Expected plain value");
  return Ready(std::move(v));
}

}  // namespace exe::future
