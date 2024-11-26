#pragma once

#include "scheduler.hpp"

namespace exe::sched::task {

template <typename F>
void Submit(IScheduler& /*scheduler*/, F /*fun*/) {
  // Not implemented
}

}  // namespace exe::sched::task
