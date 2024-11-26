#pragma once

#include "future.hpp"

#include <exe/thread/event.hpp>

#include <cstdlib>  // std::abort

namespace exe::future {

template <typename T>
T Get(Future<T>) {
  std::abort();  // Not implemented
}

}  // namespace exe::future
