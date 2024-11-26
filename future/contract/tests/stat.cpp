#include <exe/future/contract.hpp>
#include <exe/future/get.hpp>

#include <twist/sim.hpp>

#include <twist/ed/std/thread.hpp>

#include <twist/test/assert.hpp>

#include <wheels/test/framework.hpp>

#include <fmt/core.h>

static_assert(twist::build::IsolatedSim());

TEST_SUITE(ContractStat) {
  SIMPLE_TEST(Set) {
    twist::sim::sched::FairScheduler scheduler;
    twist::sim::Simulator simulator{&scheduler};

    auto result = simulator.Run([] {
      using namespace exe;  // NOLINT

      auto alloc1  = twist::sim::stat::AllocationCount();
      auto atomic1 = twist::sim::stat::AtomicCount();

      auto [f, p] = future::Contract<int>();

      std::move(p).Set(4);

      auto alloc2  = twist::sim::stat::AllocationCount();
      auto atomic2 = twist::sim::stat::AtomicCount();

      TWIST_TEST_ASSERT((alloc2 - alloc1) <= 1, "Excessive dynamic memory allocations");
      TWIST_TEST_ASSERT((atomic2 - atomic1) <= 1, "Excessive synchronization");

      std::move(f).Consume([](int) {});

      auto atomic3 = twist::sim::stat::AtomicCount();

      TWIST_TEST_ASSERT((atomic3 - atomic1) <= 1, "Excessive synchronization");
    });

    if (!result.Ok()) {
      FAIL_TEST(result.std_err);
    }
  }

  SIMPLE_TEST(Get) {
    twist::sim::sched::CoopScheduler scheduler;
    twist::sim::Simulator simulator{&scheduler};

    auto result = simulator.Run([] {
      using namespace exe;  // NOLINT

      auto alloc1  = twist::sim::stat::AllocationCount();
      auto atomic1 = twist::sim::stat::AtomicCount();

      auto [f, p] = future::Contract<int>();

      twist::ed::std::thread producer([p = std::move(p)] mutable {
        std::move(p).Set(7);
      });

      auto value = future::Get(std::move(f));
      ASSERT_EQ(value, 7);

      auto alloc2  = twist::sim::stat::AllocationCount();
      auto atomic2 = twist::sim::stat::AtomicCount();

      // Two user allocations + std::thread
      TWIST_TEST_ASSERT((alloc2 - alloc1) <= 3, "Excessive dynamic memory allocations");
      // Two user atomics + std::thread
      TWIST_TEST_ASSERT((atomic2 - atomic1) <= 3, "Excessive synchronization");

      producer.join();
    });

    if (!result.Ok()) {
      FAIL_TEST(result.std_err);
    }
  }
}

RUN_ALL_TESTS()
