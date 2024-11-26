#pragma once

#include <function2/function2.hpp>

#include "handle.hpp"

namespace exe::fiber {

using Awaiter = fu2::unique_function<void(FiberHandle)>;

}  // namespace exe::fiber
