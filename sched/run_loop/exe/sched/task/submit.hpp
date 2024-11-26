#pragma once

#include "scheduler.hpp"

namespace exe::sched::task {

template <typename F>
void Submit(IScheduler& scheduler, F fun) {
  scheduler.Submit(std::move(fun));
}

}  // namespace exe::sched::task
