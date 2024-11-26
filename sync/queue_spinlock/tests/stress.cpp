#include "../queue_spinlock.hpp"

#include <course/test/twist.hpp>
#include <course/test/time_budget.hpp>

#include <twist/test/wg.hpp>
#include <twist/test/plate.hpp>

#include <chrono>

#include <fmt/core.h>

using namespace std::chrono_literals;  // NOLINT

TEST_SUITE(StressQueueSpinlock) {
  void Test(size_t threads) {
    QueueSpinLock spinlock;
    twist::test::Plate plate;  // Guarded by spinlock

    twist::test::WaitGroup wg;

    for (size_t i = 0; i < threads; ++i) {
      wg.Add([&] {
        course::test::TimeBudget time_budget;

        while (time_budget) {
          QueueSpinLock::Guard guard{spinlock};
          {
            // Critical section
            plate.Access();
          }
        }
      });
    }

    wg.Join();

    fmt::println("Critical sections: {}", plate.AccessCount());
  }

  TWIST_STRESS_TEST(Mutex2, 5s) {
    Test(2);
  }

  TWIST_STRESS_TEST(Mutex5, 5s) {
    Test(5);
  }
}

RUN_ALL_TESTS()
