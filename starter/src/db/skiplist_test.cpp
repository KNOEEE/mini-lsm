#include "db/skiplist.h"

#include <gtest/gtest.h>

/**
 * Run $ bazel run @hedron_compile_commands//:refresh_all
 * after adding new deps
 * ref: https://github.com/hedronvision/bazel-compile-commands-extractor
 */
namespace minilsm {
typedef uint64_t Key;
struct Comparator {
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

}
}  // namespace minilsm