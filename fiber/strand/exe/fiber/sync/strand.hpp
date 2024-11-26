#pragma once

#include <exe/fiber/sched/suspend.hpp>

namespace exe::fiber {

class Strand {
 public:
  template <typename F>
  void Combine(F /*cs*/) {
    // Not implemented
  }

 private:
  //
};

}  // namespace exe::fiber
