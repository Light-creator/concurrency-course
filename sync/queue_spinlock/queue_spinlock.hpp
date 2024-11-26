#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

/*
 * Scalable Queue SpinLock
 *
 * Usage:
 *
 * QueueSpinLock spinlock;
 *
 * {
 *   QueueSpinLock::Guard guard{spinlock};  // <-- Acquire spinlock
 *   // <-- Critical section
 * }  // <-- Release spinlock (~Guard)
 *
 */

class QueueSpinLock {
 public:
  class Guard {
    friend class QueueSpinLock;

   public:
    explicit Guard(QueueSpinLock& host)
        : host_(host) {
      host_.Acquire(this);
    }

    // Non-copyable
    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;

    // Non-movable
    Guard(Guard&&) = delete;
    Guard& operator=(Guard&&) = delete;

    ~Guard() {
      host_.Release(this);
    }

   private:
    QueueSpinLock& host_;
    // ???
  };

 private:
  void Acquire(Guard* /*waiter*/) {
    // Your code goes here
  }

  void Release(Guard* /*owner*/) {
    // Your code goes here
  }

 private:
  // ???
};
