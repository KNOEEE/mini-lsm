#include "db/skiplist.h"

#include <gtest/gtest.h>

/**
 * Run $ bazel run @hedron_compile_commands//:refresh_all
 * after adding new deps
 * ref: https://github.com/hedronvision/bazel-compile-commands-extractor
 */
namespace minilsm {

using Key = uint64_t;

struct TestComparator {
  int operator()(const Key& a, const Key& b) const {
    if (a < b) {
      return -1;
    } else if (a > b) {
      return +1;
    } else {
      return 0;
    }
  }
};

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(SkipTest, InsertAndLookup) {
  Arena arena;
  TestComparator cmp;
  SkipList<Key, TestComparator> list(cmp, &arena);
  ASSERT_TRUE(!list.Contains(10));

  SkipList<Key, TestComparator>::Iterator iter(&list);
  ASSERT_TRUE(!iter.Valid());
  iter.SeekToFirst();
  ASSERT_TRUE(!iter.Valid());

  const int N = 40;
  const int R = 100;
  Random rnd(100);
  std::set<Key> keys;
  for (int i = 0; i < N; i++) {
    Key key = rnd.Next() % R;
    if (keys.insert(key).second) {
      list.Insert(key);
    }
  }

  for (int i = 0; i < R; i++) {
    if (list.Contains(i)) {
      ASSERT_EQ(keys.count(i), 1U);
    } else {
      ASSERT_EQ(keys.count(i), 0U);
    }
  }
  list.Print();
}

TEST(SkipTest, GrowAndPrint) {
  Arena arena;
  TestComparator cmp;
  SkipList<Key, TestComparator> list(cmp, &arena);

  const int R = 1000;
  Random rnd(time(0));
  std::set<Key> keys;
  for (int i = 0; i < 4; i++) {
    for (int j = i * 20; j < (i + 1) * 20; j++) {
      Key key = rnd.Next() % R;
      // Do not allow duplicate insert here
      if (keys.insert(key).second) {
        list.Insert(key);
      }
    }
    std::cout << "[Status] " << i << std::endl;
    list.Print();
  }
}
}  // namespace minilsm