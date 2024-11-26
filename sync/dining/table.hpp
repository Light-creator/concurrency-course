#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/test/plate.hpp>
#include <twist/ed/std/atomic.hpp>

#include <condition_variable>
#include <mutex>

#include <vector>

namespace dining {

using Fork = twist::ed::std::mutex;

using Plate = twist::test::Plate;
class Table {
 public:

  explicit Table(size_t num_seats)
      : num_seats_(num_seats),
        plates_(num_seats_),
        forks_(num_seats_) {

    for(size_t i=0; i<num_seats; i++) {
      sema_[i].SetCount(1);
      // fa_[i] = false;
    }
  }

  Fork& LeftFork(size_t seat) {
    return forks_[seat];
  }

  Fork& RightFork(size_t seat) {
    return forks_[ToRight(seat)];
  }

  size_t ToRight(size_t seat) const {
    return (seat + 1) % num_seats_;
  }

  void AccessPlate(size_t seat) {
    plates_[seat].Access();
  }

  size_t num_seats() { return num_seats_; }

 private:
  const size_t num_seats_;
  std::vector<Plate> plates_;
  std::vector<Fork> forks_;

public:
  // Semaphore sema_[100];
  std::mutex m_;
  twist::ed::std::mutex mu_;
  
  Semaphore sema_[10];
};
  
}  // namespace dining
