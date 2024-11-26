#include "suspend.hpp"

#include <exe/fiber/core/fiber.hpp>

namespace exe::fiber {

void Suspend(Awaiter awaiter) {
  Fiber::Self().Suspend(std::move(awaiter));
}

}  // namespace exe::fiber
