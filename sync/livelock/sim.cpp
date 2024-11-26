#include <wheels/test/framework.hpp>

// https://gitlab.com/Lipovsky/tinyfiber
#include <tf/sched/spawn.hpp>
#include <tf/sched/yield.hpp>
#include <tf/sync/wait_group.hpp>
#include <fmt/core.h>

using tf::WaitGroup;
using tf::Yield;

void LiveLock() {
  static const size_t kIterations = 100;

  size_t cs_count = 0;

  // SpinLock state
  size_t thread_count = 0;

  auto contender = [&] {
    for (size_t i = 0; i < kIterations; ++i) {
      // SpinLock::Lock
      while (thread_count++ > 0) {
        // fmt::println("{}", thread_count);
        tf::Yield();
        --thread_count;
      }
      // Spinlock acquired
    
      {
        // Critical section
        ++cs_count;
        ASSERT_TRUE_M(cs_count < 3, "Too many critical sections");
        fmt::println("{}", cs_count);
        tf::Yield();
        // End of critical section
      }

      // SpinLock::Unlock
      --thread_count;
      // Spinlock released
    }
  };

  // Spawn two fibers
  WaitGroup wg;
  wg.Spawn(contender).Spawn(contender).Wait();
};
