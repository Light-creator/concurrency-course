#include "thread_pool.hpp"

namespace exe::sched::tp::compute {

ThreadPool::ThreadPool(size_t /*threads*/) {
  // Not implemented
}

void ThreadPool::Start() {
  // Not implemented
}

ThreadPool::~ThreadPool() {
  // Not implemented
}

void ThreadPool::Submit(task::TaskBase*, task::SchedulerHint) {
  // Not implemented
}

ThreadPool* ThreadPool::Current() {
  return nullptr;  // Not implemented
}

void ThreadPool::Stop() {
  // Not implemented
}

}  // namespace exe::sched::tp::compute
