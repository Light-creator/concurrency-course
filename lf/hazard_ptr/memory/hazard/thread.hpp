#pragma once

#include "hazard_ptr.hpp"
#include "retired_ptr.hpp"
#include "limits.hpp"

namespace hazard {

struct ThreadState {
  std::array<HazardPtr, kMaxHazardPtrs> slots;
  // List of retired objects
  // ???
};

}  // namespace hazard
