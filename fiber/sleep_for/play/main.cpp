#include <asio/io_context.hpp>

#include <exe/fiber/sched/go.hpp>
#include <exe/fiber/sched/sleep_for.hpp>

#include <fmt/core.h>

using namespace exe;  // NOLINT

using namespace std::chrono_literals;  // NOLINT

int main() {
  asio::io_context scheduler;

  fiber::Go(scheduler, [] {
    for (size_t i = 0; i < 10; ++i) {
      fmt::println("Step {}", i);
      fiber::SleepFor(256ms);
    }
  });

  scheduler.run();

  return 0;
}
