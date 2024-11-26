#pragma once

#include <function2/function2.hpp>

namespace exe::future {

template <typename T>
using Callback = fu2::unique_function<void(T)>;

}  // namespace exe::future
