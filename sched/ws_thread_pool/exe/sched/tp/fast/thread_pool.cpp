#include "thread_pool.hpp"

#include <cstdlib>  // std::abort

namespace exe::sched::tp::fast {

ThreadPool::ThreadPool(size_t threads)
    : threads_(threads),
      coordinator_(threads) {
}

void ThreadPool::Start() {
  // Not implemented
}

ThreadPool::~ThreadPool() {
  // Not implemented
}

void ThreadPool::Submit(task::TaskBase* /*task*/,
                        task::SchedulerHint /*hint*/) {
  // Not implemented
}

void ThreadPool::Stop() {
  // Not implemented
}

PoolMetrics ThreadPool::Metrics() const {
  std::abort();  // Not implemented
}

ThreadPool* ThreadPool::Current() {
  return nullptr;  // Not implemented
}

}  // namespace exe::sched::tp::fast
