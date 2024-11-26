#include <exe/sched/thread_pool.hpp>
#include <exe/thread/wait_group.hpp>

#include <twist/sim.hpp>

#include <wheels/test/framework.hpp>

#include <vector>

static_assert(twist::build::IsolatedSim());

using namespace exe;  // NOLINT

TEST_SUITE(ThreadPool) {

  struct Job final : sched::task::TaskBase {
    thread::WaitGroup* wg;

    explicit Job(thread::WaitGroup& wg) {
      this->wg = &wg;
      wg.Add(1);
    }

    ~Job() {}

    void Run() noexcept override {
      wg->Done();
    }
  };

  SIMPLE_TEST(NoAllocations) {
    twist::sim::sched::RandomScheduler scheduler;
    twist::sim::Simulator simulator{&scheduler};

    auto result = simulator.Run([] {
      sched::ThreadPool pool{3};
      pool.Start();

      thread::WaitGroup wg;

      std::vector<Job> jobs;
      for (size_t i = 0; i < 128; ++i) {
        jobs.emplace_back(wg);
      }

      size_t alloc_count_1 = twist::sim::stat::AllocationCount();

      for (auto& job : jobs) {
        pool.Submit(&job);
      }
      wg.Wait();

      size_t alloc_count_2 = twist::sim::stat::AllocationCount();

      ASSERT_EQ(alloc_count_1, alloc_count_2);

      pool.Stop();
    });

    ASSERT_TRUE(result.Ok());
  }
}

RUN_ALL_TESTS()
