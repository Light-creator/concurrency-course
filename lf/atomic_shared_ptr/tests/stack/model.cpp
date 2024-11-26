#include "../../lock_free_stack.hpp"

#include <course/test/twist.hpp>
#include <course/test/checksum.hpp>

#include <twist/test/assert.hpp>
#include <twist/test/wg.hpp>
#include <twist/test/message.hpp>

static_assert(twist::build::IsolatedSim());

using TestMessage = twist::test::Message<int>;

TEST_SUITE(ModelLockFreeStack) {
  // 3 threads, Push + TryPop, <= 2 preemptions

  constexpr auto kPushPop1Params =
      course::test::twist::model::Params{
          .max_preemptions = 2
      };

  TWIST_MODEL(PushPop1, kPushPop1Params) {
    LockFreeStack<TestMessage> stack;

    course::test::CheckSum<int> s;

    twist::test::WaitGroup wg;

    wg.Add(3, [&](size_t index) {
      {
        int d = (int)index;

        stack.Push(TestMessage::New(d));
        // Checksum
        s.Produce(d);
      }

      {
        auto m = stack.TryPop();
        TWIST_TEST_ASSERT(m, "Expected message");
        int d = m->Read();
        s.Consume(d);
      }

    });

    wg.Join();

    TWIST_TEST_ASSERT(s.Validate(), "Checksum mismatch");
  }

  // 2 threads, Push + TryPop, <= 4 preemptions

  constexpr auto kPushPop2Params =
      course::test::twist::model::Params{
          .max_preemptions = 4
      };

  TWIST_MODEL(PushPop2, kPushPop2Params) {
    LockFreeStack<TestMessage> stack;

    course::test::CheckSum<int> s;

    twist::test::WaitGroup wg;

    wg.Add(2, [&](size_t index) {
      {
        int d = (int)index;

        stack.Push(TestMessage::New(d));
        // Checksum
        s.Produce(d);
      }

      {
        auto m = stack.TryPop();
        TWIST_TEST_ASSERT(m, "Expected message");
        int d = m->Read();
        s.Consume(d);
      }

    });

    wg.Join();

    TWIST_TEST_ASSERT(s.Validate(), "Checksum mismatch");
  }
}

RUN_ALL_TESTS()
