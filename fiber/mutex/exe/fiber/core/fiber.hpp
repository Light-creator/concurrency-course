#pragma once

#include "body.hpp"
#include "coroutine.hpp"
#include "scheduler.hpp"
#include "awaiter.hpp"

namespace exe::fiber {

// Fiber = stackful coroutine + scheduler

class Fiber {
 public:
  Fiber(IScheduler&, Body);

  void Suspend(Awaiter);

  void Schedule();
  void Switch();

  static Fiber& Self();

 private:
  // ???
};

}  // namespace exe::fiber
