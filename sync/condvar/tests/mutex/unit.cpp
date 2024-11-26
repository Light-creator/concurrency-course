#include "../../mutex.hpp"

#include <wheels/test/framework.hpp>

#include <course/test/cpu.hpp>

#include <twist/build.hpp>

#include <chrono>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

static_assert(twist::build::Plain());

TEST_SUITE(UnitTest) {
  SIMPLE_TEST(LockUnlock) {
    Mutex mutex;

    mutex.Lock();
    std::cout << "After lock\n";
    mutex.Unlock();
  }

  SIMPLE_TEST(LockGuard) {
    Mutex mutex;

    {
      std::lock_guard guard(mutex);
    }
  }

  SIMPLE_TEST(SequentialLockUnlock) {
    Mutex mutex;

    mutex.Lock();
    mutex.Unlock();

    mutex.Lock();
    mutex.Unlock();
  }

  SIMPLE_TEST(NoSharedLocations) {
    Mutex mutex1;

    mutex1.Lock();

    Mutex mutex2;

    mutex2.Lock();
    mutex2.Unlock();

    mutex1.Unlock();
  }

  SIMPLE_TEST(MutualExclusion) {
    Mutex mutex;
    bool cs = false;

    std::thread locker([&] {
      mutex.Lock();
      cs = true;
      std::cout << cs << " - 1\n";
      std::this_thread::sleep_for(3s);
      cs = false;
      std::cout << cs << " - 2\n";
      mutex.Unlock();
    });

    std::this_thread::sleep_for(1s);
    mutex.Lock();
    std::cout << cs << " - 3\n";
    ASSERT_FALSE(cs);
    mutex.Unlock();

    locker.join();
  }

  SIMPLE_TEST(Blocking) {
    Mutex mutex;

    // Warmup
    mutex.Lock();
    mutex.Unlock();
    std::cout << "Warmup\n";

    std::thread sleeper([&] {
      std::cout << "Sleeper start\n";
      mutex.Lock();
      std::cout << "Sleeper lock start\n";
      std::this_thread::sleep_for(3s);
      std::cout << "Sleeper lock end\n";
      mutex.Unlock();
    });

    std::thread waiter([&] {
      std::this_thread::sleep_for(1s);
      std::cout << "Waiter start\n";

      
      std::cout << "Waiter before lock\n";
      mutex.Lock();
      course::test::ThreadCPUTimer cpu_timer;
      std::cout << "Waiter\n";
      mutex.Unlock();

      auto spent = cpu_timer.Spent();

      ASSERT_TRUE(spent < 200ms);
    });

    sleeper.join();
    waiter.join();
  }
}

RUN_ALL_TESTS()
