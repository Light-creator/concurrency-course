#include <exe/sched/thread_pool.hpp>
#include <exe/thread/wait_group.hpp>

#include <wheels/test/framework.hpp>

#include <fmt/core.h>

using namespace exe;  // NOLINT

TEST_SUITE(RunLoop) {
  SIMPLE_TEST(JustWorks) {
    sched::ThreadPool pool{4};
    pool.Start();

    class DoWork : public sched::task::TaskBase {
     public:
      explicit DoWork(thread::WaitGroup& wg)
          : wg_(wg) {
      }

      void Run() noexcept override {
        wg_.Done();
      }

     private:
      thread::WaitGroup& wg_;
    };

    {
      thread::WaitGroup wg;

      DoWork set{wg};

      wg.Add(1);
      pool.Submit(&set);
      wg.Wait();
    }

    pool.Stop();
  }
}

RUN_ALL_TESTS()
