#pragma once

#include <function2/function2.hpp>

#include <cstdlib>  // std::abort

namespace hazard {

using Deleter = fu2::function_view<void(void*)>;

struct RetiredPtr {
  void* object;
  Deleter deleter;
};

template <typename T>
RetiredPtr Retired(T*) {
  std::abort();  // Not implemented
}

}  // namespace hazard
