#include "../queue_spinlock.hpp"

#include <course/test/twist.hpp>

#include <twist/test/wg.hpp>
#include <twist/test/plate.hpp>

#include <twist/assist/random.hpp>

TEST_SUITE(RandomizeQueueSpinLock) {
  TWIST_RANDOMIZE(MutexSmall, 5s) {
    QueueSpinLock spinlock;
    twist::test::Plate plate;

    twist::test::WaitGroup wg;
    wg.Add(3, [&] {
      QueueSpinLock::Guard guard(spinlock);
      plate.Access();
    });
    wg.Join();
  }

  TWIST_RANDOMIZE(MutexMedium, 5s) {
    twist::ed::std::random_device rd{};
    twist::assist::Choice choice{rd};

    const size_t threads = choice(2, 5);

    QueueSpinLock spinlock;
    twist::test::Plate plate;

    twist::test::WaitGroup wg;

    for (size_t i = 0; i < threads; ++i) {
      const size_t locks = choice(1, 5);

      wg.Add([&, locks] {
        for (size_t k = 0; k < locks; ++k) {
          QueueSpinLock::Guard guard{spinlock};
          {
            // Critical section
            plate.Access();
          }
        }
      });
    }

    wg.Join();
  }
}

RUN_ALL_TESTS()
