#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/trait/value_of.hpp>

#include <exe/future/thunk/stub.hpp>

#include <tuple>

namespace exe::future {

template <SomeFuture LeftInput, SomeFuture RightInput>
SomeFuture auto Both(LeftInput, RightInput) {
  using A = trait::ValueOf<LeftInput>;
  using B = trait::ValueOf<RightInput>;

  return thunk::Stub<std::tuple<A, B>>{};
}

}  // namespace exe::future
