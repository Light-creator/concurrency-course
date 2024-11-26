#include "run_loop.hpp"

namespace exe::sched {

void RunLoop::Submit(task::TaskBase* /*task*/) {
  // Not implemented
}

// Run tasks

size_t RunLoop::RunAtMost(size_t /*limit*/) {
  return 0;  // Not implemented
}

size_t RunLoop::Run() {
  return 0;  // Not implemented
}

}  // namespace exe::sched
