#include <exe/fiber/sched/go.hpp>
#include <exe/fiber/sched/yield.hpp>

#include "../common/run.hpp"

#include <wheels/test/framework.hpp>

using namespace exe;  // NOLINT

TEST_SUITE(Yield) {
  SIMPLE_TEST(JustWorks) {
    RunScheduler(1, [] {
      for (size_t i = 0; i < 3; ++i) {
        fiber::Yield();
      }
    });
  }

  SIMPLE_TEST(Steps) {
    RunScheduler(1, [] {
      size_t step = 0;

      fiber::Go([&] {
        for (size_t j = 1; j < 7; ++j) {
          step = j;
          fiber::Yield();
        }
      });

      for (size_t i = 1; i < 7; ++i) {
        fiber::Yield();
        ASSERT_EQ(step, i);
      }
    });
  }
}

RUN_ALL_TESTS()
