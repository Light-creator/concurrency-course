#include "inline.hpp"

namespace exe::sched {

class Inline : public task::IScheduler {
 public:
  // task::IScheduler
  void Submit(task::Task task) override {
    task();
  }
};

task::IScheduler& Inline() {
  static class Inline instance;
  return instance;
}

}  // namespace exe::sched
