#pragma once

#include <exe/sched/task/scheduler.hpp>

#include <cstddef>

namespace exe::sched {

class ThreadPool : public task::IScheduler {
 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  void Start();

  // task::IScheduler
  void Submit(task::TaskBase*) override;

  static ThreadPool* Current();

  void Stop();

 private:
  //
};

}  // namespace exe::sched
