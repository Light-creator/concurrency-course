#pragma once

#include "body.hpp"
#include "coroutine.hpp"
#include "scheduler.hpp"
#include "handler.hpp"

namespace exe::fiber {

// Fiber = stackful coroutine + scheduler

class Fiber {
 public:
  Fiber(Scheduler&, Body);

  // ???

  void Suspend(Handler);

  static Fiber& Self();

 private:
  // ???
};

}  // namespace exe::fiber
