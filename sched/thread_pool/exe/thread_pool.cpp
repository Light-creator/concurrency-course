#include <exe/thread_pool.hpp>

#include <twist/ed/static/thread_local/ptr.hpp>

#include <wheels/core/panic.hpp>

ThreadPool::ThreadPool(size_t /*threads*/) {
  // Not implemented
}

void ThreadPool::Start() {
  // Not implemented
}

ThreadPool::~ThreadPool() {
  // Not implemented
}

void ThreadPool::Submit(Task /*task*/) {
  // Not implemented
}

ThreadPool* ThreadPool::Current() {
  return nullptr;  // Not implemented
}

void ThreadPool::Stop() {
  // Not implemented
}
