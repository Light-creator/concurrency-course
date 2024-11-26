#pragma once

#include <twist/ed/std/atomic.hpp>

namespace exe::future::state {

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
  //
};

}  // namespace exe::future::state
