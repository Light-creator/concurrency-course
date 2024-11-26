#include "fiber.hpp"

#include <cstdlib>

namespace exe::fiber {

Fiber::Fiber(IScheduler&, Body) {
  // Not implemented
}

Fiber& Fiber::Self() {
  std::abort();  // Not implemented
}

}  // namespace exe::fiber
