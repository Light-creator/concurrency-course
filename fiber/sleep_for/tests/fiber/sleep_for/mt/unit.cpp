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
  SIMPLE_TEST(JustWorks) {
    RunScheduler(/*threads=*/4, []() {
      for (size_t i = 0; i < 17; ++i) {
        fiber::SleepFor(100ms);
        std::cout << i << std::endl;
      }
    });
  }
}

RUN_ALL_TESTS()
