#include <exe/sched/run_loop.hpp>

#include <exe/fiber/sched/go.hpp>
#include <exe/fiber/sched/yield.hpp>

#include <fmt/core.h>

using namespace exe;  // NOLINT

int main() {
  sched::RunLoop loop;

  for (size_t i = 1; i <= 3; ++i) {
    fiber::Go(loop, [i] {
      fmt::println("Fiber #{} started", i);

      for (size_t j = 0; j < 7; ++j) {
        fiber::Yield();

        fmt::println("Fiber #{}, iter = {}", i, j);
      }
    });
  }

  // Deterministic execution

  loop.RunAtMost(3);

  loop.Run();

  return 0;
}
