#pragma once

#include "fwd.hpp"
#include "thread.hpp"
#include "guard.hpp"

#include <cstdlib>

namespace hazard {

class Mutator {
 public:
  Mutator(Manager* manager, ThreadState* thread)
      : manager_(manager),
        thread_(thread) {
  }

  PtrGuard GetHazardPtr(size_t index) {
    return PtrGuard{&(thread_->slots[index])};
  }

  template <typename T>
  void Retire(T* /*object*/) {
    // Not implemented
  }

 private:
  [[maybe_unused]] Manager* manager_;
  ThreadState* thread_;
};

}  // namespace hazard
