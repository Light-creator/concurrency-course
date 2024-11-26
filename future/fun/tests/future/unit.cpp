#include <exe/sched/thread_pool.hpp>
#include <exe/sched/run_loop.hpp>

#include <exe/future/make/contract.hpp>
#include <exe/future/make/value.hpp>
#include <exe/future/make/submit.hpp>
#include <exe/future/make/return.hpp>
#include <exe/future/make/just.hpp>

#include <exe/future/make/result/ok.hpp>
#include <exe/future/make/result/err.hpp>

#include <exe/future/combine/seq/map.hpp>
#include <exe/future/combine/seq/flatten.hpp>
#include <exe/future/combine/seq/flat_map.hpp>
#include <exe/future/combine/seq/via.hpp>

#include <exe/future/combine/seq/result/map_ok.hpp>
#include <exe/future/combine/seq/result/and_then.hpp>
#include <exe/future/combine/seq/result/or_else.hpp>
#include <exe/future/combine/seq/result/as.hpp>

#include <exe/future/combine/concur/first.hpp>
#include <exe/future/combine/concur/all.hpp>

#include <exe/future/combine/concur/result/first.hpp>
#include <exe/future/combine/concur/result/all.hpp>

#include <exe/future/terminate/get.hpp>
#include <exe/future/terminate/detach.hpp>

#include <exe/result/make/ok.hpp>
#include <exe/result/make/err.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/test/util/cpu_timer.hpp>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <tuple>

using namespace exe;

using namespace std::chrono_literals;

std::error_code TimeoutError() {
  return std::make_error_code(std::errc::timed_out);
}

std::error_code IoError() {
  return std::make_error_code(std::errc::io_error);
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

struct NonDefaultConstructible {
  NonDefaultConstructible(int) {};
};

TEST_SUITE(Futures) {
  SIMPLE_TEST(ContractValue) {
    auto [f, p] = future::Contract<std::string>();

    std::move(p).Set("Hi");
    std::string s = future::Get(std::move(f));

    ASSERT_EQ(s, "Hi");
  }

  SIMPLE_TEST(ContractDetach) {
    auto [f, p] = future::Contract<int>();

    std::move(f) | future::Detach();
    std::move(p).Set(1);
  }

  SIMPLE_TEST(ContractMoveOnlyValue) {
    auto [f, p] = future::Contract<MoveOnly>();

    std::move(p).Set(MoveOnly{});
    auto v = future::Get(std::move(f));

    WHEELS_UNUSED(v);
  }

  SIMPLE_TEST(ContractNonDefaultConstructibleValue) {
    auto [f, p] = future::Contract<NonDefaultConstructible>();

    std::move(p).Set({128});
    future::Get(std::move(f));
  }

  SIMPLE_TEST(Value) {
    auto f = future::Value(std::string("Hello"));
    auto s = future::Get(std::move(f));

    ASSERT_EQ(s, "Hello");
  }

  SIMPLE_TEST(Just) {
    auto j = future::Just();
    auto u = future::Get(std::move(j));
    ASSERT_EQ(u, unit);
  }

  SIMPLE_TEST(SubmitPool) {
    sched::ThreadPool pool{4};
    pool.Start();

    auto compute = future::Submit(pool, [] -> int {
      return 11;
    });

    int v = future::Get(std::move(compute));

    ASSERT_EQ(v, 11);

    pool.Stop();
  }

  SIMPLE_TEST(SubmitLoop) {
    sched::RunLoop loop;

    bool done = false;

    auto f = future::Submit(loop, [&] {
      done = true;
      return unit;
    });

    std::move(f) | future::Detach();

    ASSERT_FALSE(done);

    loop.Run();

    ASSERT_TRUE(done);
  }

  SIMPLE_TEST(SubmitPoolWait) {
    sched::ThreadPool pool{4};
    pool.Start();

    auto f = future::Submit(pool, [] {
      std::this_thread::sleep_for(1s);
      return 11;
    });

    wheels::ProcessCPUTimer timer;

    auto v = future::Get(std::move(f));

    ASSERT_TRUE(timer.Spent() < 100ms);

    ASSERT_EQ(v, 11);

    pool.Stop();
  }

  SIMPLE_TEST(Map) {
    auto f = future::Value(1)
             | future::Map([](int v) {
                 return v + 1;
               });

    auto v = std::move(f) | future::Get();

    ASSERT_EQ(v, 2);
  }

  SIMPLE_TEST(Flatten) {
    sched::RunLoop loop;

    auto ff = future::Submit(loop, [&loop] {
      return future::Submit(loop, [] {
        return 7;
      });
    });

    future::Future<int> f = std::move(ff) | future::Flatten();

    bool ok = false;

    std::move(f) | future::Map([&ok](int v) -> Unit {
      ASSERT_EQ(v, 7);
      ok = true;
      return {};
    }) | future::Detach();

    ASSERT_FALSE(ok);

    loop.Run();

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(FlatMap) {
    sched::RunLoop loop;

    future::Future<int> f = future::Submit(loop, [] {
                              return 23;
                            })
                            | future::FlatMap([&](int v) {
                                return future::Submit(loop, [v] {
                                  return v + 5;
                                });
                              })
                            | future::Map([](int v) {
                                return v + 1;
                              });

    bool ok = true;

    std::move(f)
        | future::Map([&](int v) {
            ASSERT_EQ(v, 29);
            ok = true;
            return unit;
          })
        | future::Detach();

    loop.Run();

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(MapOk) {
    {
      auto f = future::Ok(1)
               | future::MapOk([](int v) {
                   return v + 1;
                 });

      auto r = future::Get(std::move(f));
      ASSERT_TRUE(r);
      ASSERT_EQ(*r, 2);
    }

    {
      auto failure = [] -> future::TryFuture<int> {
        return future::Err(TimeoutError());
      };

      auto f = failure() | future::MapOk([](int) {
                   FAIL_TEST("Unreachable");
                   return unit;
                 });

      auto r = future::Get(std::move(f));
      ASSERT_FALSE(r);
    }

  }

  SIMPLE_TEST(AndThen) {
    auto f = future::Ok(std::string("ok"))
             | future::AndThen([](std::string s) {
                 return future::Ok(s + "!");
               })
             | future::AndThen([](std::string s) {
                 return future::Ok(s + "!");
               });

    auto r = future::Get(std::move(f));

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, "ok!!");
  }

  SIMPLE_TEST(OrElse) {
    auto failure = [] -> future::TryFuture<std::string> {
      return future::Err(IoError());
    };

    auto f = failure()
             | future::OrElse([](Error e) {
                 ASSERT_EQ(e, IoError());
                 return future::Ok(std::string("fallback"));
               });

    auto r = future::Get(std::move(f));

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, "fallback");
  }

  SIMPLE_TEST(TryPipeline) {
    auto [f, p] = future::Contract<int>();

    auto g = std::move(f)
             | future::Map([](int v) {
                 return v + 1;
               })
             | future::Map([](int v) {
                 return v + 2;
               })
             | future::AsTryFuture()
             | future::OrElse([](Error) {
                 FAIL_TEST("Unreachable");
                 return future::Ok(111);
               })
             | future::AndThen([](int /*v*/) -> future::TryFuture<int> {
                 return future::Err(TimeoutError());
               })
             | future::MapOk([](int v) {
                 FAIL_TEST("Unreachable");
                 return v + 1;
               })
             | future::OrElse([](Error) {
                 return future::Ok(17);
               })
             | future::MapOk([](int v) {
                 return v + 1;
               });

    std::move(p).Set(3);

    auto r = future::Get(std::move(g));

    ASSERT_TRUE(r);
    ASSERT_EQ(*r, 18);
  }

  SIMPLE_TEST(Via) {
    sched::RunLoop loop1;
    sched::RunLoop loop2;

    size_t steps = 0;

    future::Just()
        | future::Via(loop1)
        | future::Map([&](Unit) {
            ++steps;
            return unit;
          })
        | future::Map([](Unit) {
            return unit;
          })
        | future::Via(loop2)
        | future::Map([&](Unit) {
            ++steps;
            return unit;
          })
        | future::Map([&](Unit) {
            ++steps;
            return unit;
          })
        | future::Via(loop1)
        | future::Map([&](Unit) {
            ++steps;
            return unit;
          })
        | future::FlatMap([&](Unit) {
            ++steps;
            return future::Value(1);
          })
        | future::Map([&](int v) {
            ASSERT_EQ(v, 1);
            ++steps;
            return unit;
          })
        | future::Detach();

    ASSERT_EQ(loop1.Run(), 2);
    ASSERT_EQ(steps, 1);
    ASSERT_GE(loop2.Run(), 2);
    ASSERT_EQ(steps, 3);
    ASSERT_GE(loop1.Run(), 3);
    ASSERT_EQ(steps, 6);
  }

  SIMPLE_TEST(First1) {
    auto [f1, p1] = future::Contract<int>();
    auto [f2, p2] = future::Contract<int>();

    auto first = future::First(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(first)
        | future::Map([&ok](int v) {
            ASSERT_EQ(v, 1);
            ok = true;
            return unit;
          })
        | future::Detach();

    std::move(p1).Set(1);

    ASSERT_TRUE(ok);

    std::move(p2).Set(2);
  }

  SIMPLE_TEST(First2) {
    auto [f1, p1] = future::Contract<int>();
    auto [f2, p2] = future::Contract<int>();

    auto first = future::First(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(first)
        | future::Map([&ok](int v) {
            ASSERT_EQ(v, 2);
            ok = true;
            return unit;
          })
        | future::Detach();

    std::move(p2).Set(2);

    ASSERT_TRUE(ok);

    std::move(p1).Set(1);
  }

  SIMPLE_TEST(FirstOk1) {
    auto [f1, p1] = future::Contract<Result<int>>();
    auto [f2, p2] = future::Contract<Result<int>>();

    auto first = future::FirstOk(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(first)
        | future::MapOk([&ok](int v) {
            ASSERT_EQ(v, 2);
            ok = true;
            return unit;
          })
        | future::Detach();

    std::move(p2).Set(result::Ok(2));

    ASSERT_TRUE(ok);

    std::move(p1).Set(result::Ok(1));
  }

  SIMPLE_TEST(FirstOk2) {
    auto [f1, p1] = future::Contract<Result<int>>();
    auto [f2, p2] = future::Contract<Result<int>>();

    auto first = future::FirstOk(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(first)
        | future::MapOk([&ok](int v) {
            ASSERT_EQ(v, 29);
            ok = true;
            return unit;
          })
        | future::Detach();

    std::move(p1).Set(result::Err(TimeoutError()));
    std::move(p2).Set(result::Ok(29));

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(FirstOk3) {
    auto [f1, p1] = future::Contract<Result<int>>();
    auto [f2, p2] = future::Contract<Result<int>>();

    auto first = future::FirstOk(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(first)
        | future::MapOk([&ok](int v) {
            ASSERT_EQ(v, 31);
            ok = true;
            return unit;
          })
        | future::Detach();

    std::move(p2).Set(result::Err(IoError()));
    std::move(p1).Set(result::Ok(31));

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(FirstFailure) {
    auto [f1, p1] = future::Contract<Result<int>>();
    auto [f2, p2] = future::Contract<Result<int>>();

    auto first = future::FirstOk(std::move(f1), std::move(f2));

    bool fail = false;

    std::move(first)
        | future::OrElse([&](Error e) -> future::TryFuture<int> {
            ASSERT_EQ(e, TimeoutError());
            fail = true;
            return future::Err(e);
          })
        | future::Detach();

    std::move(p2).Set(result::Err(TimeoutError()));
    std::move(p1).Set(result::Err(TimeoutError()));

    ASSERT_TRUE(fail);
  }

  SIMPLE_TEST(Both) {
    auto [f1, p1] = future::Contract<int>();
    auto [f2, p2] = future::Contract<int>();

    auto both = future::Both(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(both)
        | future::Map([&ok](auto tuple) {
            auto [x, y] = tuple;
            ASSERT_EQ(x, 1);
            ASSERT_EQ(y, 2);
            ok = true;
            return unit;
          })
        | future::Detach();

    std::move(p2).Set(2);
    std::move(p1).Set(1);

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(BothOk) {
    auto [f1, p1] = future::Contract<Result<int>>();
    auto [f2, p2] = future::Contract<Result<int>>();

    auto both = future::BothOk(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(both)
        | future::MapOk([&ok](auto tuple) {
            auto [x, y] = tuple;
            ASSERT_EQ(x, 2);
            ASSERT_EQ(y, 1);
            ok = true;
            return unit;
          })
        | future::Detach();

    std::move(p2).Set(result::Ok(1));
    std::move(p1).Set(result::Ok(2));

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(BothFailure1) {
    auto [f1, p1] = future::Contract<Result<int>>();
    auto [f2, p2] = future::Contract<Result<int>>();

    auto both = future::BothOk(std::move(f1), std::move(f2));

    bool fail = false;

    std::move(both)
        | future::OrElse([&fail](Error e) -> future::TryFuture<std::tuple<int, int>> {
            ASSERT_EQ(e, TimeoutError());
            fail = true;
            return future::Err(e);
          })
        | future::Detach();

    std::move(p1).Set(result::Err(TimeoutError()));

    ASSERT_TRUE(fail);

    std::move(p2).Set(result::Ok(7));
  }

  SIMPLE_TEST(BothFailure2) {
    auto [f1, p1] = future::Contract<Result<int>>();
    auto [f2, p2] = future::Contract<Result<int>>();

    auto both = future::BothOk(std::move(f1), std::move(f2));

    bool fail = false;

    std::move(both)
        | future::OrElse([&fail](Error e) -> future::TryFuture<std::tuple<int, int>> {
            ASSERT_EQ(e, IoError());
            fail = true;
            return future::Err(e);
          })
        | future::Detach();

    std::move(p2).Set(result::Err(IoError()));

    ASSERT_TRUE(fail);

    std::move(p1).Set(result::Ok(4));
  }

  SIMPLE_TEST(BothTypes) {
    auto [f1, p1] = future::Contract<std::string>();
    auto [f2, p2] = future::Contract<std::tuple<int, int>>();

    auto both = future::Both(std::move(f1), std::move(f2));

    bool ok = false;

    std::move(both)
        | future::Map([&ok](auto tuple) {
            auto [x, y] = tuple;

            ASSERT_EQ(x, "3");

            std::tuple<int, int> t = {1, 2};
            ASSERT_EQ(y, t);

            ok = true;
            return unit;
          })
        | future::Detach();

    std::move(p2).Set({1, 2});
    std::move(p1).Set("3");

    ASSERT_TRUE(ok);
  }

  SIMPLE_TEST(DoNotWait1) {
    sched::ThreadPool pool{4};
    pool.Start();

    std::atomic_bool submit = false;

    auto f = future::Submit(pool,
                             [&] {
                               std::this_thread::sleep_for(1s);
                               submit = true;
                               return 11;
                             })
             | future::Map([](int v) {
                 return v + 1;
               });

    ASSERT_FALSE(submit);

    auto v = future::Get(std::move(f));

    ASSERT_TRUE(submit);
    ASSERT_EQ(v, 12);

    pool.Stop();
  }

  SIMPLE_TEST(DoNotWait2) {
    sched::ThreadPool pool{4};
    pool.Start();

    std::atomic_bool submit = false;

    auto f = future::Submit(pool,
                             [&] {
                               std::this_thread::sleep_for(1s);
                               submit = true;
                               return 31;
                             })
             | future::FlatMap([&](int v) {
                 return future::Submit(pool, [v] {
                   return v + 1;
                 });
               });

    ASSERT_FALSE(submit);

    auto v = future::Get(std::move(f));

    ASSERT_TRUE(submit);
    ASSERT_EQ(v, 32);

    pool.Stop();
  }

  SIMPLE_TEST(Inline1) {
    sched::RunLoop loop;

    bool ok = false;

    future::Just()
        | future::Via(loop)
        | future::Map([&](Unit) {
            ok = true;
            return unit;
          })
        | future::Detach();

    size_t tasks = loop.Run();
    ASSERT_TRUE(ok);
    ASSERT_EQ(tasks, 1);
  }

  SIMPLE_TEST(Inline2) {
    sched::RunLoop loop;

    auto v = future::Get(future::Value(1) | future::Via(loop));

    ASSERT_EQ(v, 1);
  }

  SIMPLE_TEST(Inline3) {
    sched::RunLoop loop;

    bool flat_map = false;
    bool map1 = false;
    bool map2 = false;

    future::Just()
        | future::Via(loop)
        | future::FlatMap([&](Unit) {
            flat_map = true;
            return future::Value(unit);
          })
        | future::Map([&](Unit u) {
            map1 = true;
            return u;
          })
        | future::Map([&](Unit u) {
            map2 = true;
            return u;
          })
        | future::Detach();

    ASSERT_TRUE(loop.RunNext());
    ASSERT_TRUE(flat_map);
    ASSERT_FALSE(map1);

    ASSERT_TRUE(loop.RunNext());
    ASSERT_TRUE(map1);
    ASSERT_FALSE(map2);

    ASSERT_EQ(loop.Run(), 1);
    ASSERT_TRUE(map2);
  }

  SIMPLE_TEST(Inline4) {
    sched::RunLoop loop;

    future::Submit(loop, [&] {
      return future::Submit(loop, [] {
        return 19;
      });
    }) | future::Flatten() | future::Detach();

    size_t tasks = loop.Run();
    ASSERT_EQ(tasks, 2);
  }

  SIMPLE_TEST(Inline5) {
    sched::RunLoop loop;

    auto [f1, p1] = future::Contract<Result<int>>();
    auto [f2, p2] = future::Contract<Result<int>>();

    auto first = future::FirstOk(
        std::move(f1) | future::Via(loop),
        std::move(f2) | future::Via(loop));

    bool ok = false;

    std::move(first)
        | future::MapOk([&ok](int v) {
            ASSERT_EQ(v, 31);
            ok = true;
            return unit;
          })
        | future::Detach();

    std::move(p2).Set(result::Err(IoError()));
    std::move(p1).Set(result::Ok(31));

    size_t tasks = loop.Run();
    ASSERT_EQ(tasks, 0);

    ASSERT_TRUE(ok);
  }
}

RUN_ALL_TESTS()
