#include <exe/sched/thread_pool.hpp>

#include <exe/fiber/sched/go.hpp>
#include <exe/fiber/sync/channel/buffered.hpp>

#include <exe/thread/wait_group.hpp>

#include <fmt/core.h>

using namespace exe;  // NOLINT

int main() {
  sched::ThreadPool pool{4};
  pool.Start();

  thread::WaitGroup wg;

  wg.Add(2);

  fiber::BufferedChannel<int> messages{4};

  // Захватываем канал по значению, владение каналом – разделяемое
  fiber::Go(pool, [&wg, messages] mutable {
    // Producer
    for (int i = 0; i < 128; ++i) {
      messages.Send(i);
    }

    // Отправляем poison pill
    messages.Send(-1);

    wg.Done();
  });

  fiber::Go(pool, [&wg, messages] mutable {
    // Consumer
    while (true) {
      int v = messages.Recv();
      if (v == -1) {
        break;  // Poison pill
      }
      fmt::println("Recv -> {}", v);
    }

    wg.Done();
  });

  wg.Wait();

  pool.Stop();

  return 0;
}
