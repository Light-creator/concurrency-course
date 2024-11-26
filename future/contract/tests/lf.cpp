#include <exe/future/contract.hpp>

#include <course/test/twist.hpp>

static_assert(twist::build::IsolatedSim());

TEST_SUITE(LockFreeContract) {
  SIMPLE_TEST(ConsumeSet) {
    // TODO
  }
}

RUN_ALL_TESTS()
