#pragma once

#include "hazard_ptr.hpp"

#include <twist/ed/std/atomic.hpp>

#include <twist/trace/scope.hpp>

namespace hazard {

struct PtrGuard {
  friend class Mutator;

 public:
  PtrGuard(const PtrGuard&);
  PtrGuard& operator=(const PtrGuard&);

  // Non-movable
  PtrGuard(PtrGuard&&) = delete;
  PtrGuard& operator=(PtrGuard&&) = delete;

  template <typename T>
  T* Protect(twist::ed::std::atomic<T*>& /*atomic_ptr*/,
             twist::trace::Scope = twist::trace::Scope(guard, "Protect")) {
    return nullptr;  // Not implemented
  }

  template <typename T>
  void Announce(T* /*ptr*/,
                twist::trace::Scope = twist::trace::Scope(guard, "Announce")) {
    // Not implemented
  }

  void Reset(twist::trace::Scope = twist::trace::Scope(guard, "Reset")) {
    // Not implemented
  }

  ~PtrGuard() {
    Reset();
  }

 private:
  explicit PtrGuard(HazardPtr* slot)
      : slot_(slot) {
  }

 private:
  // Tracing
  static inline twist::trace::Domain guard{"HazardPtr"};

 private:
  HazardPtr* slot_;
  // ???
};

}  // namespace hazard
