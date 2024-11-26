#pragma once

#include <exe/sched/task/scheduler.hpp>

#include "queues/global_queue.hpp"
#include "worker.hpp"
#include "coordinator.hpp"
#include "metrics.hpp"

// std::random_device
#include <twist/ed/std/random.hpp>

#include <deque>

namespace exe::sched::tp::fast {

// Scalable work-stealing scheduler for
// fibers, stackless coroutines and futures

class ThreadPool : public task::IScheduler {
  friend class Worker;

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
  void Submit(task::TaskBase*, task::SchedulerHint) override;

  void Stop();

  // After Stop
  PoolMetrics Metrics() const;

  static ThreadPool* Current();

 private:
  const size_t threads_;
  std::deque<Worker> workers_;
  Coordinator coordinator_;
  GlobalQueue global_tasks_;
  PoolMetrics metrics_;
  twist::ed::std::random_device random_device_;
};

}  // namespace exe::sched::tp::fast
