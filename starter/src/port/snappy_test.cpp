#include "port_stdcxx.h"

#include <gtest/gtest.h>
namespace minilsm {

TEST(SnappyTest, Compress) {
  std::string s = "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";
  std::string result;
  bool ok = port::Snappy_Compress(s.data(), s.size(), &result);
  ASSERT_TRUE(ok);
  ASSERT_LT(result.size(), s.size());
  // std::cout << result.size() << std::endl;
  size_t ulength = 0;
  ok = port::Snappy_GetUncompressedLength(result.data(), 
                                          result.size(), &ulength);
  ASSERT_TRUE(ok);
            
  char* uncompressed = new char[ulength];
  ok = port::Snappy_Uncompress(result.data(), result.size(), uncompressed);
  ASSERT_EQ(s, std::string(uncompressed));
}

}