#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

namespace exe::thread {

class Event {
 public:
  // One-shot
  void Fire() {
    // Not implemented
  }

  void Wait() {
    // Not implemented
  }
};

}  // namespace exe::thread
