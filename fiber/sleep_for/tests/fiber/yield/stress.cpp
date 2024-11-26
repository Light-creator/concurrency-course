#include <exe/fiber/sched/go.hpp>
#include <exe/fiber/sched/yield.hpp>

#include "../common/run.hpp"
#include "../common/test.hpp"

#include <wheels/test/framework.hpp>

#include <chrono>

using namespace exe;  // NOLINT

using namespace std::chrono_literals;  // NOLINT

TEST_SUITE(Yield) {
  TEST(Stress, wheels::test::TestOptions().TimeLimit(5s)) {
    RunScheduler(4, [] {
      for (size_t i = 0; i < 127; ++i) {
        fiber::Go([] {
          while (KeepRunning()) {
            fiber::Yield();
          }
        });
      }
    });
  }
}

RUN_ALL_TESTS()
