#include "philosopher.hpp"

#include <twist/test/inject_fault.hpp>
#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

#include <fmt/core.h>

namespace dining {

Philosopher::Philosopher(Table& table, size_t seat)
    : table_(table),
      seat_(seat),
      left_fork_(table_.LeftFork(seat)),
      right_fork_(table_.RightFork(seat)) {

}

void Philosopher::Eat() {
  AcquireForks();
  EatWithForks();
  ReleaseForks();
}

// Acquire left_fork_ and right_fork_
void Philosopher::AcquireForks() {
  if(seat_ < (seat_+1)%table_.num_seats()) {
    left_fork_.lock();
    right_fork_.lock();
  } else {
    right_fork_.lock();
    left_fork_.lock();
  }
}

void Philosopher::EatWithForks() {
  table_.AccessPlate(seat_);
  table_.AccessPlate(table_.ToRight(seat_));
  ++meals_;
}

// Release left_fork_ and right_fork_
void Philosopher::ReleaseForks() {
  right_fork_.unlock();
  left_fork_.unlock();
}

void Philosopher::Think() {
  // Random pause or context switch
  twist::test::InjectFault();
}

}  // namespace dining
