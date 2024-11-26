#pragma once

#include <exe/fiber/core/body.hpp>
#include <exe/fiber/core/hint.hpp>
#include <exe/fiber/core/scheduler.hpp>

namespace exe::fiber {

// Considered harmful

void Go(IScheduler&, Body);

void Go(Body);

// Hints

inline void Go(IScheduler& scheduler, Body body, Hint) {
  Go(scheduler, std::move(body));
}

inline void Go(Body body, Hint) {
  Go(std::move(body));
}

}  // namespace exe::fiber
