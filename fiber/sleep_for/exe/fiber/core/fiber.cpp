#include "fiber.hpp"

#include <cstdlib>

namespace exe::fiber {

Fiber::Fiber(Scheduler&, Body) {
  // Not implemented
}

void Fiber::Suspend(Handler) {
  // Not implemented
}

Fiber& Fiber::Self() {
  std::abort();  // Not implemented
}

}  // namespace exe::fiber
