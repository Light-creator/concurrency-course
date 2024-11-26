#pragma once

#include <function2/function2.hpp>

namespace exe::fiber {

class Fiber;

// ~ System call handler
using Handler = fu2::unique_function<void(Fiber*)>;

}  // namespace exe::fiber
