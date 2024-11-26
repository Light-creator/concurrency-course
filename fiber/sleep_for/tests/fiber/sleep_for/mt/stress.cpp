#include <exe/fiber/sched/go.hpp>
#include <exe/fiber/sched/yield.hpp>
#include <exe/fiber/sched/sleep_for.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/test/current.hpp>

#include "../../common/run.hpp"
#include "../../common/test.hpp"

using namespace exe;  // NOLINT

using namespace std::chrono_literals;  // NOLINT

//////////////////////////////////////////////////////////////////////

TEST_SUITE(SleepForMultiThread) {

  void StressTest1(size_t fibers) {
    RunScheduler(4, [fibers]() {
      for (size_t i = 0; i < fibers; ++i) {
        fiber::Go([i] {
          for (size_t j = 0; KeepRunning(); ++j) {
            fiber::SleepFor(((i + j) % 5) * 1ms);

            if (j % 11 == 0) {
              fiber::Yield();
            }
          }
        });
      }
    });
  }

  TEST(Stress1_1, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest1(/*fibers=*/2);
  }

  TEST(Stress1_2, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest1(/*fibers=*/5);
  }

  TEST(Stress1_3, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest1(/*fibers=*/10);
  }

  void StressTest2(size_t fibers) {
    while (KeepRunning()) {
      RunScheduler(4, [fibers]() {
        for (size_t i = 0; i < fibers; ++i) {
          fiber::Go([i] {
            fiber::SleepFor((i % 2) * 1ms);
          });
        }
      });
    }
  }

  TEST(Stress_2_1, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest2(/*fibers=*/1);
  }

  TEST(Stress_2_2, wheels::test::TestOptions().TimeLimit(5s)) {
    StressTest2(/*fibers=*/2);
  }
}

RUN_ALL_TESTS()
