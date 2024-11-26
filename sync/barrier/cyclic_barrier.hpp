#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/fmt/print.hpp>

#include <cstddef>

class CyclicBarrier {
public:
  explicit CyclicBarrier(size_t c): lim(c) {}

  void ArriveAndWait() {
    twist::ed::std::unique_lock<twist::ed::std::mutex> lk(m);
    // twist::ed::fmt::println("Count {}", count+1);

    size_t prev_phase = phase;

    if(++count == lim) {
      count = 0;
      phase++;
      cv.notify_all();
    } else {
      cv.wait(lk, [this, prev_phase]{ return this->phase != prev_phase; });
    }
  }

private: 
  size_t count = 0;
  size_t lim = 1;
  size_t phase = 0;

  twist::ed::std::mutex m;
  twist::ed::std::condition_variable cv;
};
