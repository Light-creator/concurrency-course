#include "worker.hpp"
#include "thread_pool.hpp"

#include <twist/ed/static/thread_local/ptr.hpp>

namespace exe::sched::tp::fast {

Worker::Worker(ThreadPool& host, size_t index)
    : host_(host),
      index_(index) {
  // Not implemented
}

void Worker::Start() {
  thread_.emplace([this] {
    Work();
  });
}

void Worker::Join() {
  thread_->join();
}

void Worker::Push(task::TaskBase* /*task*/, task::SchedulerHint /*hint*/) {
  // Not implemented
}

task::TaskBase* Worker::PickTask() {
  // Poll in order:
  // * [%61] global queue
  // * LIFO slot
  // * local queue
  // * global queue
  // * work stealing
  // then
  //   park worker

  return nullptr;  // Not implemented
}

void Worker::Work() {
  // Not implemented

  while (task::TaskBase* next = PickTask()) {
    next->Run();
  }
}

}  // namespace exe::sched::tp::fast
