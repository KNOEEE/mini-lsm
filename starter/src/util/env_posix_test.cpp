#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_set>
#include <vector>

#include "minilsm/env.h"
#include "port/port.h"
#include "util/env_posix_test_helper.h"

#include <gtest/gtest.h>

namespace minilsm {

static const int kReadOnlyFileLimit = 4;
static const int kMMapLimit = 4;

class EnvPosixTest : public testing::Test {
 public:
  static void SetFileLimits(int read_only_file_limit, int mmap_limit) {
    EnvPosixTestHelper::SetReadOnlyFDLimit(read_only_file_limit);
    EnvPosixTestHelper::SetReadOnlyMMapLimit(mmap_limit);
  }
  EnvPosixTest() {
    SetFileLimits(kReadOnlyFileLimit, kMMapLimit);
    env_ = Env::Default();
  }
  Env* env_;
};

TEST_F(EnvPosixTest, TestOpenOnRead) {
  // Write some test data to a single file that will be opened |n| times.
  std::string test_file = "./open_on_read.txt";
  FILE* f = fopen(test_file.c_str(), "we");
  ASSERT_TRUE(f != nullptr);
  const char kFileData[] = "abcdefghijklmnopqrstuvwxyz";
  fputs(kFileData, f);
  fclose(f);

  // Open test file some number above the sum of the two limits to force
  // open-on-read behavior of POSIX Env leveldb::RandomAccessFile.
  const int kNumFiles = kReadOnlyFileLimit + kMMapLimit + 5;
  RandomAccessFile* files[kNumFiles] = {0};
  for (int i = 0; i < kNumFiles; i++) {
    ASSERT_TRUE(env_->NewRandomAccessFile(test_file, &files[i]).ok());
  }
  char scratch;
  Slice read_result;
  for (int i = 0; i < kNumFiles; i++) {
    ASSERT_TRUE(files[i]->Read(i, 1, &read_result, &scratch).ok());
    ASSERT_EQ(kFileData[i], read_result[0]);
  }
  for (int i = 0; i < kNumFiles; i++) {
    delete files[i];
  }
  ASSERT_TRUE(env_->DeleteFile(test_file).ok());
}

}  // namespace minilsm 