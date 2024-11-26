#include <exe/sched/run_loop.hpp>

#include <fmt/core.h>

using namespace exe;  // NOLINT

struct Hello : sched::task::TaskBase {
  void Run() noexcept override {
    fmt::println("I'm intrusive task");
  }
};

int main() {
  // No dynamic memory allocations here!

  sched::RunLoop loop;

  {
    Hello hello;
    loop.Submit(&hello);
    loop.Run();
  }

  return 0;
}
