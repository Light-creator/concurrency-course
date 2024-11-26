#include <exe/sched/run_loop.hpp>
#include <exe/sched/thread_pool.hpp>

#include <exe/fiber/sched/go.hpp>
#include <exe/fiber/sched/await.hpp>

#include <exe/future/make/contract.hpp>
#include <exe/future/make/ready.hpp>
#include <exe/future/combine/seq/via.hpp>

#include <exe/thread/wait_group.hpp>

#include <wheels/test/framework.hpp>

using namespace exe;  // NOLINT

TEST_SUITE(FiberAwait) {
  SIMPLE_TEST(ReadyFuture) {
    sched::RunLoop loop;

    bool done = false;

    fiber::Go(loop, [&done] mutable {
      int v = fiber::Await(future::Ready(42));
      ASSERT_EQ(v, 42);
      done = true;
    });

    ASSERT_LE(loop.Run(), 2);
    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(SuspendFiber) {
    sched::RunLoop loop;

    auto [f, p] = future::Contract<int>();

    bool done = false;

    fiber::Go(loop, [&done, f = std::move(f)] mutable {
      int v = fiber::Await(std::move(f));
      ASSERT_EQ(v, 8);
      done = true;
    });

    ASSERT_EQ(loop.Run(), 1);

    std::move(p).Set(8);

    ASSERT_EQ(loop.Run(), 1);
    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(ThreadPool) {
    sched::ThreadPool pool{1};
    pool.Start();

    auto [f, p] = future::Contract<int>();

    thread::WaitGroup wg;

    wg.Add(1);
    fiber::Go(pool, [&wg, f = std::move(f)] mutable {
      int v = fiber::Await(std::move(f));
      ASSERT_EQ(v, 41);
      wg.Done();
    });

    std::move(p).Set(41);

    wg.Wait();

    pool.Stop();
  }

  SIMPLE_TEST(Inline) {
    sched::RunLoop loop;

    auto [f, p] = future::Contract<int>();

    fiber::Go(loop, [f = std::move(f) | future::Via(loop)] mutable {
      int v = fiber::Await(std::move(f));
      ASSERT_EQ(v, 11);
    });

    loop.Run();  // Suspended

    std::move(p).Set(11);

    {
      size_t tasks = loop.Run();
      ASSERT_EQ(tasks, 1);
    }
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
    sched::RunLoop loop;

    fiber::Go(loop, [] {
      [[maybe_unused]] auto v = fiber::Await(future::Ready(MoveOnly{}));
    });

    loop.Run();
  }

  struct NonDefaultConstructible {
    NonDefaultConstructible(int) {}
  };

  SIMPLE_TEST(NonDefaultConstructibleValue) {
    sched::RunLoop loop;

    fiber::Go(loop, [] {
      [[maybe_unused]] auto v = fiber::Await(future::Ready(NonDefaultConstructible{6}));
    });

    loop.Run();
  }
}

RUN_ALL_TESTS()
