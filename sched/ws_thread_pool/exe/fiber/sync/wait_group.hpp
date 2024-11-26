#pragma once

#include <exe/fiber/sched/suspend.hpp>

#include <cstddef>

namespace exe::fiber {

class WaitGroup {
 public:
  void Add(size_t /*count*/) {
    // Not implemented
  }

  void Done() {
    // Not implemented
  }

  void Wait() {
    // Not implemented
  }

 private:
  //
};

}  // namespace exe::fiber
