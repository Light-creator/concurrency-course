#include <exe/sched/thread_pool.hpp>

#include <exe/fiber/sched/go.hpp>

#include <exe/fiber/sync/strand.hpp>
#include <exe/fiber/sync/wait_group.hpp>

#include <exe/thread/wait_group.hpp>

#include <exe/util/defer.hpp>

#include <fmt/core.h>

using namespace exe;  // NOLINT

int main() {
  sched::ThreadPool scheduler{4};
  scheduler.Start();

  thread::WaitGroup example;
  example.Add(1);

  fiber::Go(scheduler, [&example] {
    fiber::WaitGroup wg;

    fiber::Strand mutex;
    size_t cs = 0;

    for (size_t i = 0; i < 512; ++i) {
      wg.Add(1);

      fiber::Go([&] {
        Defer defer([&] {
          wg.Done();
        });

        for (size_t j = 0; j < 1024; ++j) {
          mutex.Combine([&] {
            ++cs;
          });
        }
      });
    }

    wg.Wait();

    fmt::println("# critical sections: {}", cs);

    example.Done();
  });

  example.Wait();

  scheduler.Stop();

  return 0;
}
