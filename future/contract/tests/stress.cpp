#include <exe/future/contract.hpp>
#include <exe/future/get.hpp>

#include <twist/ed/std/thread.hpp>

#include <course/test/twist.hpp>
#include <course/test/time_budget.hpp>

#include <fmt/core.h>

using namespace exe;  // NOLINT

TEST_SUITE(StressContract) {
  TWIST_STRESS_TEST(GetSet, 10s) {
    course::test::TimeBudget budget;
    int iter = 0;

    while (budget) {
      auto [f, p] = future::Contract<std::string>();

      auto test_message = std::to_string(iter);

      twist::ed::std::thread producer([p = std::move(p), test_message]() mutable {
        std::move(p).Set(test_message);
      });

      twist::ed::std::thread consumer([f = std::move(f), test_message]() mutable {
        std::string message = future::Get(std::move(f));
        ASSERT_EQ(message, test_message);
      });

      producer.join();
      consumer.join();

      ++iter;
    }

    fmt::println("Iterations: {}", iter);
  }

  class Barrier {
   public:
    explicit Barrier(size_t count)
        : left_(count) {
    }

    void ArriveAndWait() {
      if (left_.fetch_sub(1) > 1) {
        while (left_.load() > 0) {
          twist::ed::std::this_thread::yield();
        }
      }
    }

   private:
    twist::ed::std::atomic_size_t left_;
  };

  TWIST_STRESS_TEST(ConsumeSet, 10s) {
    course::test::TimeBudget budget;
    int iter = 0;

    while (budget) {
      auto [f, p] = future::Contract<std::string>();

      auto test_message = std::to_string(iter);

      Barrier start{2};

      twist::ed::std::thread producer([&, p = std::move(p)]() mutable {
        start.ArriveAndWait();

        std::move(p).Set(test_message);
      });

      start.ArriveAndWait();

      std::move(f).Consume([test_message](auto message) mutable {
        ASSERT_EQ(message, test_message);
      });

      producer.join();

      ++iter;
    }

    fmt::println("Iterations: {}", iter);
  }
}

RUN_ALL_TESTS()
