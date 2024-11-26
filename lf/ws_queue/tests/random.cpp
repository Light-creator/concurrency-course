#include "../work_stealing_queue.hpp"

#include <course/test/twist.hpp>

#include <twist/ed/std/random.hpp>

#include <twist/assist/shared.hpp>

#include <twist/test/assert.hpp>
#include <twist/test/checksum.hpp>
#include <twist/test/lock_free.hpp>
#include <twist/test/wg.hpp>

#include <vector>
#include <array>
#include <random>

/////////////////////////////////////////////////////////////////////

struct Task {
  uint64_t ctx;

  twist::assist::Shared<int> shared;

  Task(uint64_t r)
    : ctx(r), shared(1) {
  }
};

/////////////////////////////////////////////////////////////////////

template <size_t Threads, size_t QueueCapacity, size_t StealBufferSize>
void Test() {
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

      twist::test::LockFreeScope lf;

      for (size_t iter = 0; iter < 3; ++iter) {
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
          twist::test::Progress();  // Grab

          for (size_t k = 0; k < stolen; ++k) {
            Task* task = buffer[k];

            TWIST_TEST_ASSERT(task != nullptr, "Task expected");

            task->shared.Write(1);

            bool ok = local_queue.TryPush(task);
            TWIST_TEST_ASSERT(ok, "Unexpected queue overflow");
            twist::test::Progress();  // TryPush
          }
        }

        skip_stealing:

        {
          // Local work

          size_t streak = thread_twister() % QueueCapacity;

          for (size_t k = 0; k < streak; ++k) {
            Task* task = local_queue.TryPop();
            twist::test::Progress();  // TryPop

            if (task == nullptr) {
              break;
            }

            task->shared.Write(1);
            {
              bool ok = local_queue.TryPush(task);
              TWIST_TEST_ASSERT(ok, "Unexpected queue overflow");
            }
            twist::test::Progress();  // TryPush
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

/////////////////////////////////////////////////////////////////////

TEST_SUITE(RandomizeWorkStealingQueue) {
  TWIST_RANDOMIZE(Steal1, 10s) {
    Test<2, 5, 3>();
  }

  TWIST_RANDOMIZE(Steal2, 10s) {
    Test<3, 7, 5>();
  }

  TWIST_RANDOMIZE(Steal3, 10s) {
    Test<4, 10, 6>();
  }

  TWIST_RANDOMIZE(Steal4, 10s) {
    Test<5, 15, 7>();
  }
}

RUN_ALL_TESTS()
