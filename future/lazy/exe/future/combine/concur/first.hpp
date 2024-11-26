#pragma once

#include <exe/future/type/future.hpp>

#include <exe/future/trait/value_of.hpp>

#include <exe/future/thunk/stub.hpp>

namespace exe::future {

template <SomeFuture LeftInput, SomeFuture RightInput>
SomeFuture auto First(LeftInput, RightInput) {
  using T = trait::ValueOf<LeftInput>;

  return thunk::Stub<T>{};
}

}  // namespace exe::future
