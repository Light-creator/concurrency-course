#pragma once

#include <exe/future/make/value.hpp>

#include <exe/result/type/unit.hpp>

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

inline Future<Unit> Just() {
  return Value(unit);
}

}  // namespace exe::future
