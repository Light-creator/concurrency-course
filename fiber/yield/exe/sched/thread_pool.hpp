#pragma once

#include "task.hpp"

#include <cstddef>

namespace exe::sched {

class ThreadPool {
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

  void Submit(Task);

  static ThreadPool* Current();

  void Stop();

 private:
  //
};

}  // namespace exe::sched
