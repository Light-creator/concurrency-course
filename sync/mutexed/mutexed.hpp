#pragma once

#include <atomic>
#include <memory>
#include <twist/ed/std/mutex.hpp>

#include <utility>

template <typename T, class Mutex = twist::ed::std::mutex>
class OwnerMutexed {
public:
  OwnerMutexed(T* object, Mutex* m_): mutex_(m_), object_(object) {}
  
  ~OwnerMutexed() {
    mutex_->unlock();
  }
  
  T* operator->() {
    return object_;
  }

  T& operator*() {
    return *object_;
  }

private:
  Mutex* mutex_;
  T* object_;
};

template <typename T, class Mutex = twist::ed::std::mutex>
class Mutexed {
  using OwnerRef = OwnerMutexed<T>;

 public:
  template <typename... Args>
  explicit Mutexed(Args&&... args)
      : object_(std::forward<Args>(args)...) {
  }

  OwnerRef Acquire() {
    mutex_.lock();
    return OwnerRef(&object_, &mutex_);  
  }

  OwnerRef operator->() { 
    mutex_.lock();
    return OwnerRef(&object_, &mutex_); 
  }

private:
  T object_;
  Mutex mutex_;  // Guards access to object_
};

template <typename T>
auto Acquire(Mutexed<T>& object) {
  return object.Acquire();
}
