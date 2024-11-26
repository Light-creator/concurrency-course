#include <exe/sched/run_loop.hpp>

#include <exe/fiber/sched/go.hpp>
#include <exe/fiber/sched/yield.hpp>

#include <twist/sim.hpp>

#include <twist/test/assert.hpp>

#include <wheels/test/framework.hpp>

static_assert(twist::build::IsolatedSim());

using namespace exe;  // NOLINT

TEST_SUITE(Yield) {
  SIMPLE_TEST(NoAllocations) {
    twist::sim::sched::CoopScheduler scheduler;
    twist::sim::Simulator simulator{&scheduler};

    auto result = simulator.Run([] {
      sched::RunLoop loop;

      for (size_t i = 0; i < 3; ++i) {
        fiber::Go(loop, [] {
          for (size_t i = 0; i < 1111; ++i) {
            fiber::Yield();
          }
        });
      }

      loop.RunAtMost(10);

      size_t alloc_count_1 = twist::sim::stat::AllocationCount();

      loop.Run();

      size_t alloc_count_2 = twist::sim::stat::AllocationCount();

      TWIST_TEST_ASSERT(alloc_count_1 == alloc_count_2, "Dynamic memory allocations");
    });

    ASSERT_TRUE_M(result.Ok(), result.std_err);
  }
}

RUN_ALL_TESTS()
