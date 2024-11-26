#pragma once

#include <exe/sched/task/scheduler.hpp>

#include <cstddef>

namespace exe::sched {

// Single-threaded task queue

class RunLoop : public task::IScheduler {
 public:
  RunLoop() = default;

  // Non-copyable
  RunLoop(const RunLoop&) = delete;
  RunLoop& operator=(const RunLoop&) = delete;

  // Non-movable
  RunLoop(RunLoop&&) = delete;
  RunLoop& operator=(RunLoop&&) = delete;

  // task::IScheduler
  void Submit(task::Task) override;

  // Run tasks

  size_t RunAtMost(size_t limit);

  bool RunNext() {
    return RunAtMost(1) == 1;
  }

  size_t Run();

  bool IsEmpty() const {
    return false;  // Not implemented
  }

  bool NonEmpty() const {
    return !IsEmpty();
  }

 private:
  //
};

}  // namespace exe::sched
