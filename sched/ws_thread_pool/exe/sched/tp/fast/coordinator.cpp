#include "coordinator.hpp"

namespace exe::sched::tp::fast {

bool Coordinator::ShouldWakeWorker() const {
  return false;  // Not implemented
}

void Coordinator::WakeWorker() {
  // Not implemented
}

}  // namespace exe::sched::tp::fast
