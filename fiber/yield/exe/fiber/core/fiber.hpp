#pragma once

#include "body.hpp"
#include "coroutine.hpp"
#include "scheduler.hpp"

namespace exe::fiber {

// Fiber = stackful coroutine + scheduler

class Fiber {
 public:
  Fiber(Scheduler&, Body);

  // ???

  static Fiber& Self();

 private:
  // ???
};

}  // namespace exe::fiber
