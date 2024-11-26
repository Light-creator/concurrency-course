#pragma once

#include <exe/fiber/sched/go.hpp>

#include <asio/io_context.hpp>

#include <wheels/test/framework.hpp>

#include <vector>
#include <thread>

template <typename F>
void RunScheduler(size_t threads, F init) {
  asio::io_context scheduler;

  bool done = false;

  // Spawn initial fiber
  exe::fiber::Go(scheduler, [init, &done]() {
    init();
    done = true;
  });

  std::vector<std::thread> workers;

  for (size_t i = 1; i < threads; ++i) {
    workers.emplace_back([&scheduler] {
      scheduler.run();
    });
  }

  scheduler.run();

  // Join runners
  for (auto& t : workers) {
    t.join();
  }

  ASSERT_TRUE(done);
}
