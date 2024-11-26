#include "../function.hpp"

#include <wheels/test/framework.hpp>

#include <fmt/core.h>

#include <vector>

TEST_SUITE(Function) {
  SIMPLE_TEST(JustWorks) {
    bool flag = false;

    UniqueFunction f([&flag] {
      flag = true;
    });

    f();

    ASSERT_TRUE(flag);
  }

  SIMPLE_TEST(Step) {
    int count = 0;

    UniqueFunction step([&count] {
      ++count;
    });

    step();
    ASSERT_EQ(count, 1);

    step();
    ASSERT_EQ(count, 2);
  }

  SIMPLE_TEST(Move) {
    bool flag = false;

    UniqueFunction f([&flag] {
      flag = true;
    });

    UniqueFunction g{std::move(f)};

    ASSERT_FALSE(flag);

    g();

    ASSERT_TRUE(flag);
  }

  SIMPLE_TEST(MoveAssignment) {
    bool flag = false;

    UniqueFunction f([&flag] {
      flag = true;
    });

    UniqueFunction g([] {});

    g = std::move(f);

    ASSERT_FALSE(flag);

    g();

    ASSERT_TRUE(flag);
  }

  SIMPLE_TEST(TaskQueue) {
    using Task = UniqueFunction;

    std::vector<Task> tasks;

    int x = 1;
    int y = 2;
    int z = 3;

    tasks.emplace_back([x] {
      fmt::println("{}", x);
    });
    tasks.emplace_back([x, y] {
      fmt::println("{}", x + y);
    });
    tasks.emplace_back([x, y, z] {
      fmt::println("{}", x + y + z);
    });

    for (auto& task : tasks) {
      task();
    }
  }
}

RUN_ALL_TESTS()
