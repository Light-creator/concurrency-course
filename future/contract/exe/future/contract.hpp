#pragma once

#include "future.hpp"
#include "promise.hpp"

#include <cstdlib>  // std::abort
#include <tuple>

namespace exe::future {

template <typename T>
std::tuple<Future<T>, Promise<T>> Contract() {
  std::abort();  // Not implemented
}

}  // namespace exe::future
