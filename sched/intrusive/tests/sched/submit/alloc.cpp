#include <exe/sched/run_loop.hpp>
#include <exe/sched/task/submit.hpp>

#include <twist/sim.hpp>

#include <twist/test/assert.hpp>

#include <wheels/test/framework.hpp>

static_assert(twist::build::IsolatedSim());

using namespace exe;  // NOLINT

TEST_SUITE(Submit) {
  SIMPLE_TEST(SmallAllocation) {
    twist::sim::sched::CoopScheduler scheduler;
    twist::sim::Simulator simulator{&scheduler};

    auto result = simulator.Run([] {
      sched::RunLoop loop;

      size_t bytes_alloc_1 = twist::sim::stat::TotalBytesAllocated();

      bool done = false;

      sched::task::Submit(loop, [&done] {
        done = true;
      });

      TWIST_TEST_ASSERT(!done, "Before loop.Run()");

      loop.Run();

      TWIST_TEST_ASSERT(done, "Missing task");

      size_t bytes_alloc_2 = twist::sim::stat::TotalBytesAllocated();

      TWIST_TEST_ASSERT((bytes_alloc_2 - bytes_alloc_1) <= 32, "Suspicious allocations");
    });

    ASSERT_TRUE_M(result.Ok(), result.std_err);
  }
}

RUN_ALL_TESTS()
