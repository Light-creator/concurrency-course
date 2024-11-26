#include "../../stamped_ptr.hpp"

#include <wheels/test/framework.hpp>

struct Node {
  int datum;
};

TEST_SUITE(StampedPtr) {
  SIMPLE_TEST(Init) {
    Node n{13};

    StampedPtr ptr{&n, 17};

    ASSERT_EQ(ptr.raw_ptr, &n);
    ASSERT_EQ(ptr.stamp, 17);
  }

  SIMPLE_TEST(Access) {
    Node n{13};

    StampedPtr ptr{&n, 17};

    ASSERT_EQ(ptr->datum, 13);
    ASSERT_EQ((*ptr).datum, 13);
  }

  SIMPLE_TEST(OperatorBool) {
    Node n{4};

    StampedPtr<Node> null_ptr{nullptr, 0};
    ASSERT_FALSE(null_ptr);

    StampedPtr ptr{&n, 1};
    ASSERT_TRUE(ptr);
  }

  SIMPLE_TEST(Stamp) {
    Node n{11};

    StampedPtr ptr{&n, 6};

    {
      auto inc = ptr.IncrementStamp();
      ASSERT_EQ(inc.raw_ptr, &n);
      ASSERT_EQ(inc.stamp, 7);
    }

    {
      auto dec = ptr.DecrementStamp();
      ASSERT_EQ(dec.raw_ptr, &n);
      ASSERT_EQ(dec.stamp, 5);
    }
  }

  SIMPLE_TEST(Comparison) {
    Node n1{1};
    Node n2{2};

    StampedPtr<Node> p1{&n1, 7};
    StampedPtr<Node> p2{&n1, 8};
    StampedPtr<Node> p3{&n2, 7};
    StampedPtr<Node> p4{&n2, 7};

    ASSERT_NE(p1, p2);
    ASSERT_NE(p1, p3);
    ASSERT_EQ(p3, p4);
  }
}

TEST_SUITE(AtomicStampedPtr) {
  SIMPLE_TEST(InitLoad) {
    Node n{2};
    AtomicStampedPtr<Node> asp{{&n, 14}};

    auto sp = asp.Load();

    ASSERT_EQ(sp, StampedPtr(&n, 14));
  }

  SIMPLE_TEST(Store) {
    AtomicStampedPtr<Node> asp({nullptr, 0});

    Node n{5};
    asp.Store({&n, 3});

    auto sp = asp.Load();
    ASSERT_EQ(sp, StampedPtr(&n, 3));
  }

  SIMPLE_TEST(Exchange) {
    Node n1{3};
    Node n2{7};

    AtomicStampedPtr<Node> asp({&n1, 3});

    auto sp = asp.Exchange({&n2, 4});

    ASSERT_EQ(sp, StampedPtr(&n1, 3));
    ASSERT_EQ(asp.Load(), StampedPtr(&n2, 4));
  }

  SIMPLE_TEST(CompareExchangeWeak) {
    Node n1{53};
    Node n2{37};

    AtomicStampedPtr<Node> asp({&n1, 11});

    {
      // Success
      StampedPtr e{&n1, 11};
      while (!asp.CompareExchangeWeak(e, e.IncrementStamp())) {
        // Spurious failure
      }

      ASSERT_EQ(asp.Load(), StampedPtr(&n1, 12));
    }

    {
      // Failure
      for (size_t i = 0; i < 17; ++i) {
        StampedPtr e{&n2, 11};
        auto success = asp.CompareExchangeWeak(e, e.IncrementStamp());
        ASSERT_FALSE(success);
      }
    }
  }
}

RUN_ALL_TESTS()
