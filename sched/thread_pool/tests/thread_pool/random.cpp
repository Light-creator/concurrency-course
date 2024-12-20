#include <exe/thread_pool.hpp>
#include <exe/wait_group.hpp>

#include <twist/ed/std/atomic.hpp>

#include <course/test/twist.hpp>

#include <twist/assist/assert.hpp>
#include <twist/assist/random.hpp>

TEST_SUITE(RandomThreadPool) {
  TWIST_RANDOMIZE(Group, 5s) {
    twist::ed::std::random_device rd;
    twist::assist::Choice choice{rd};

    size_t workers = choice(2, 5);

    ThreadPool pool{workers};
    pool.Start();

    {
      WaitGroup wg;

      size_t tasks = choice(1, 8);

      for (size_t i = 0; i < tasks; ++i) {
        wg.Add(1);

        pool.Submit([&wg] {
          wg.Done();
        });
      }

      wg.Wait();
    }

    pool.Stop();
  }

  TWIST_RANDOMIZE(Concurrent, 5s) {
    twist::ed::std::random_device rd;
    twist::assist::Choice choice{rd};

    size_t workers = choice(2, 5);

    ThreadPool pool{workers};
    pool.Start();

    {
      WaitGroup wg;

      size_t todo = choice(1, 5);
      twist::ed::std::atomic_size_t done{0};

      for (size_t i = 0; i < todo; ++i) {
        wg.Add(1);

        pool.Submit([&] {
          wg.Add(1);
          ThreadPool::Current()->Submit([&] {
            done.fetch_add(1);
            wg.Done();
          });

          wg.Done();
        });
      }

      wg.Wait();

      TWIST_ASSERT(done.load() == todo, "Missing tasks");
    }

    pool.Stop();
  }
}

RUN_ALL_TESTS()
