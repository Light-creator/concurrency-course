#include <exe/future/contract.hpp>
#include <exe/future/get.hpp>

#include <exe/result/result.hpp>

#include <wheels/test/framework.hpp>

#include <course/test/cpu.hpp>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

using namespace exe;  // NOLINT

using namespace std::chrono_literals;  // NOLINT

TEST_SUITE(Contract) {
  SIMPLE_TEST(ConsumeReady) {
    auto [f, p] = future::Contract<int>();
    std::move(p).Set(42);

    bool consumed = false;

    // Run here
    std::move(f).Consume([&](int v) {
      ASSERT_EQ(v, 42);
      consumed = true;
    });

    ASSERT_TRUE(consumed);
  }

  SIMPLE_TEST(ConsumeLater) {
    auto [f, p] = future::Contract<std::string>();

    std::atomic_bool consumed = false;

    // Consumer
    std::move(f).Consume([&](std::string m) {
      ASSERT_EQ(m, "Hello");
      consumed = true;
    });

    // Producer
    std::this_thread::sleep_for(1s);
    std::move(p).Set("Hello");  // Run callback

    while (!consumed) {
      std::this_thread::yield();
    }
  }

  SIMPLE_TEST(GetReady) {
    auto [f, p] = future::Contract<std::string>();
    std::move(p).Set("Hello");
    auto m = future::Get(std::move(f));
    ASSERT_EQ(m, "Hello");
  }

  SIMPLE_TEST(GetWait) {
    auto [f, p] = future::Contract<int>();

    std::thread producer([p = std::move(p)] mutable {
      // Producer
      std::this_thread::sleep_for(1s);
      std::move(p).Set(7);
    });

    {
      course::test::ThreadCPUTimer timer;

      // Consumer
      auto v = future::Get(std::move(f));

      ASSERT_EQ(v, 7);

      ASSERT_TRUE(timer.Spent() < 128ms);
    }

    producer.join();
  }

  struct MoveOnly {
    MoveOnly() = default;

    // Movable
    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;

    // Non-copyable
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
  };

  SIMPLE_TEST(MoveOnlyValue) {
    auto [f, p] = future::Contract<MoveOnly>();

    std::move(p).Set({});
    std::move(f).Consume([](MoveOnly /*ignore*/) {});
  }

  struct NonDefaultConstructible {
    NonDefaultConstructible(int) {}
  };

  SIMPLE_TEST(NonDefaultConstructibleValue) {
    auto [f, p] = future::Contract<NonDefaultConstructible>();

    std::move(p).Set({42});
    std::move(f).Consume([](NonDefaultConstructible /*ignore*/) {});
  }

  std::error_code IoError() {
    return std::make_error_code(std::errc::io_error);
  }

  SIMPLE_TEST(ResultOk) {
    auto [f, p] = future::Contract<Result<int>>();

    std::thread producer([p = std::move(p)] mutable {
      std::move(p).Set(3);
    });

    auto r = future::Get(std::move(f));
    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 3);

    producer.join();
  }

  SIMPLE_TEST(ResultErr) {
    auto [f, p] = future::Contract<Result<int>>();

    std::thread producer([p = std::move(p)] mutable {
      std::move(p).Set(std::unexpected(IoError()));
    });

    auto r = future::Get(std::move(f));
    ASSERT_FALSE(r);

    producer.join();
  }
}

RUN_ALL_TESTS()
