#pragma once

#include <twist/ed/std/atomic.hpp>

namespace exe::future {

// Wait-free
class RendezvousStateMachine {
 public:
  // true means rendezvous
  bool Produce() {
    return false;  // Not implemented
  }

  // true means rendezvous
  bool Consume() {
    return false;  // Not implemented
  }

 private:
  twist::ed::std::atomic_uint32_t state_{0};
};

}  // namespace exe::future
