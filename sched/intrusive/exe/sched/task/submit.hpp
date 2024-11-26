#pragma once

#include "scheduler.hpp"

#include <cstdlib>

namespace exe::sched::task {

template <typename F>
void Submit(IScheduler& /*scheduler*/, F /*fun*/) {
  std::abort();  // Not implemented
}

}  // namespace exe::sched::task
