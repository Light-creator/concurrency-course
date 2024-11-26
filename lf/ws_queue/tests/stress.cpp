#include "../work_stealing_queue.hpp"

#include <course/test/twist.hpp>
#include <course/test/time_budget.hpp>

#include <twist/ed/std/random.hpp>

#include <twist/test/assert.hpp>
#include <twist/test/checksum.hpp>
#include <twist/test/wg.hpp>

#include <array>
#include <random>

/////////////////////////////////////////////////////////////////////

struct Task {
  uint64_t ctx;
};

/////////////////////////////////////////////////////////////////////

template <size_t Threads, size_t QueueCapacity, size_t StealBufferSize>
void StressTest() {
  twist::ed::std::random_device rd;
  std::mt19937_64 test_twister{rd()};

  std::array<WorkStealingQueue<Task, QueueCapacity>, Threads> queues;

  twist::test::CheckSum<uint64_t> checksum;

  // Set up

  for (size_t i = 0; i < Threads; ++i) {
    size_t init = test_twister() % QueueCapacity;
    for (size_t k = 0; k < init; ++k) {
      auto r = test_twister();
      queues[i].TryPush(new Task{r});
      checksum.Produce(r);
    }
  }

  // Run

  twist::test::WaitGroup wg;

  for (size_t i = 0; i < Threads; ++i) {
    wg.Add([&, index = i, seed = test_twister()] {
      std::mt19937_64 thread_twister{seed};

      auto& local_queue = queues[index];

      course::test::TimeBudget time_budget;
      while (time_budget) {
        {
          size_t victim = thread_twister() % Threads;

          if (victim == index) {
            goto skip_stealing;
          }

          // Estimate free space in local queue
          size_t space = local_queue.SpaceLowerBound();

          Task* buffer[StealBufferSize];
          size_t to_steal =
              std::min((size_t)thread_twister() % StealBufferSize, space);

          size_t stolen = queues[victim].Grab({buffer, to_steal});

          for (size_t k = 0; k < stolen; ++k) {
            bool ok = local_queue.TryPush(buffer[k]);
            TWIST_TEST_ASSERT(ok, "Unexpected queue overflow");
          }
        }

        skip_stealing:

        {
          // Local work

          size_t streak = thread_twister() % QueueCapacity;

          for (size_t k = 0; k < streak; ++k) {
            Task* task = local_queue.TryPop();
            if (task == nullptr) {
              break;
            }
            bool ok = local_queue.TryPush(task);
            TWIST_TEST_ASSERT(ok, "Unexpected queue overflow");
          }
        }
      }
    });
  }

  wg.Join();

  // Tear down

  for (size_t i = 0; i < Threads; ++i) {
    while (true) {
      Task* task = queues[i].TryPop();
      if (task == nullptr) {
        break; // Empty queue
      }
      checksum.Consume(task->ctx);
      delete task;
    }
  }

  TWIST_TEST_ASSERT(checksum.Validate(), "Checksum mismatch");
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(StressWorkStealingQueue) {
  TWIST_STRESS_TEST(Steal1, 5s) {
    StressTest<2, 5, 3>();
  }

  TWIST_STRESS_TEST(Steal2, 5s) {
    StressTest<4, 16, 7>();
  }

  TWIST_STRESS_TEST(Steal3, 5s) {
    StressTest<4, 33, 9>();
  }

  TWIST_STRESS_TEST(Steal4, 5s) {
    StressTest<4, 128, 15>();
  }
}

RUN_ALL_TESTS()
