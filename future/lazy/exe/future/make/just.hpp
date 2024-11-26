#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/thunk/stub.hpp>

#include <exe/unit.hpp>

namespace exe::future {

/*
 * Ready unit value
 * https://en.wikipedia.org/wiki/Unit_type
 *
 * Usage:
 *
 * auto f = future::Just()
 *          | future::Via(pool)
 *          | future::Map([](Unit) { return 4; });
 *
 */

inline Future<Unit> auto Just() {
  return thunk::Stub<Unit>{};
}

}  // namespace exe::future
