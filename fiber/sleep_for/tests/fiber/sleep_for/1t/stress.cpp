#include <exe/fiber/sched/go.hpp>
#include <exe/fiber/sched/sleep_for.hpp>

#include <wheels/test/framework.hpp>

#include "../../common/run.hpp"
#include "../../common/test.hpp"

using namespace exe;  // NOLINT
using namespace std::chrono_literals;  // NOLINT

class WaitGroup {
 public:
  void Add(size_t count) {
    count_ += count;
  }

  void Done() {
    --count_;
  }

  void Wait() {
    while (count_ > 0) {
      fiber::SleepFor(1ms);
    }
  }

 private:
  size_t count_{0};
};

TEST_SUITE(SleepForOneThread) {
  SIMPLE_TEST(Stress) {
    RunScheduler(/*threads=*/1, [] {
      WaitGroup wg;

      for (size_t i = 0; i < 100; ++i) {
        wg.Add(1);

        fiber::Go([&]() {
          while (KeepRunning()) {
            fiber::SleepFor(3ms);
          }
          wg.Done();
        });
      }

      wg.Wait();
    });
  }
}

RUN_ALL_TESTS()
