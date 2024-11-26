#include "../work_stealing_queue.hpp"

#include <wheels/test/framework.hpp>

struct Task {
  int ctx;
};

template <size_t Capacity>
void Cleanup(WorkStealingQueue<Task, Capacity>& queue) {
  while (Task* task = queue.TryPop()) {
    delete task;
  }
}

TEST_SUITE(WorkStealingQueue) {
  SIMPLE_TEST(JustWorks) {
    WorkStealingQueue<Task, 17> queue;

    ASSERT_TRUE(queue.TryPush(new Task{17}));

    {
      Task* task = queue.TryPop();

      ASSERT_TRUE(task != nullptr);
      ASSERT_EQ(task->ctx, 17);

      delete task;
    }

    ASSERT_EQ(queue.TryPop(), nullptr);  // Empty
  }

  SIMPLE_TEST(Fifo) {
    WorkStealingQueue<Task, 128> queue;

    for (int i = 0; i < 64; ++i) {
      ASSERT_TRUE(queue.TryPush(new Task{i}));
    }

    for (int i = 0; i < 64; ++i) {
      Task* task = queue.TryPop();

      ASSERT_TRUE(task != nullptr);
      ASSERT_EQ(task->ctx, i);

      delete task;
    }

    ASSERT_FALSE(queue.TryPop());  // Empty
  }

  SIMPLE_TEST(Capacity) {
    WorkStealingQueue<Task, 33> queue;

    for (int i = 0; i < 33; ++i) {
      ASSERT_TRUE(queue.TryPush(new Task{i}));
    }

    {
      Task* overflow = new Task{33};

      ASSERT_FALSE(queue.TryPush(overflow));
      ASSERT_FALSE(queue.TryPush(overflow));

      delete overflow;
    }

    Cleanup(queue);
  }

  SIMPLE_TEST(WrapAround) {
    WorkStealingQueue<Task, 12> queue;

    for (int i = 0; i < 6; ++i) {
      ASSERT_TRUE(queue.TryPush(new Task{i}));
    }

    for (int i = 6; i < 256; ++i) {
      ASSERT_TRUE(queue.TryPush(new Task{i}));
      {
        Task* task = queue.TryPop();

        ASSERT_TRUE(task != nullptr);
        ASSERT_EQ(task->ctx, i - 6);

        delete task;
      }
    }

    Cleanup(queue);
  }

  SIMPLE_TEST(Grab1) {
    WorkStealingQueue<Task, 17> queue;

    for (int i = 0; i < 7; ++i) {
      queue.TryPush(new Task{i});
    }

    Task* buffer[5];

    {
      size_t count = queue.Grab(buffer);
      ASSERT_EQ(count, 5);

      for (size_t i = 0; i < count; ++i) {
        ASSERT_EQ(buffer[i]->ctx, (int)i);
        delete buffer[i];
      }
    }

    {
      size_t count = queue.Grab(buffer);
      ASSERT_EQ(count, 2);

      ASSERT_EQ(buffer[0]->ctx, 5);
      ASSERT_EQ(buffer[1]->ctx, 6);

      delete buffer[0];
      delete buffer[1];
    }
  }

  SIMPLE_TEST(Grab2) {
    WorkStealingQueue<Task, 5> queue;

    // Push 3 tasks
    for (int i = 0; i < 3; ++i) {
      ASSERT_TRUE(queue.TryPush(new Task{i}));
    }

    // Pop 3 tasks
    for (int i = 0; i < 3; ++i) {
      Task* task = queue.TryPop();
      ASSERT_TRUE(task != nullptr);
      delete task;
    }

    // Push 5 tasks
    for (int i = 4; i < 4 + 5; ++i) {
      ASSERT_TRUE(queue.TryPush(new Task{i}));
    }

    {
      Task* buffer[5];

      // Grab at most 5 tasks
      size_t count = queue.Grab(buffer);
      ASSERT_EQ(count, 5);

      for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(buffer[i]->ctx, 4 + (int)i);
        delete buffer[i];
      }
    }
  }

  SIMPLE_TEST(SpaceUpperBound) {
    WorkStealingQueue<Task, 8> queue;

    ASSERT_EQ(queue.SpaceLowerBound(), 8);

    for (size_t i = 1; i <= 8; ++i) {
      queue.TryPush(new Task{(int)i});
      ASSERT_EQ(queue.SpaceLowerBound(), 8 - i);
    }

    {
      Task* task = queue.TryPop();
      ASSERT_TRUE(task != nullptr);
      ASSERT_EQ(queue.SpaceLowerBound(), 1);
      delete task;
    }

    Cleanup(queue);

    ASSERT_EQ(queue.SpaceLowerBound(), 8);
  }
}

RUN_ALL_TESTS()
