#pragma once

#if defined(__TWIST_SIM_ISOLATION__)

#include <sure/stack/new.hpp>

namespace exe::fiber {

// Adapt to deterministic simulation
using Stack = ::sure::NewStack;

}  // namespace exe::fiber

#else

#include <sure/stack/mmap.hpp>

namespace exe::fiber {

using Stack = ::sure::MmapStack;

}  // namespace exe::fiber

#endif
