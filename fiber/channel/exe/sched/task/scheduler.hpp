#pragma once

#include "task.hpp"

namespace exe::sched::task {

struct IScheduler {
  virtual ~IScheduler() = default;

  virtual void Submit(Task) = 0;
};

}  // namespace exe::sched::task
