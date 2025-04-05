#include "format.h"

#include <gtest/gtest.h>
namespace minilsm {

TEST(FormatTest, Construct) {
  BlockHandle bh;
  uint64_t offset = bh.offset();
  char* byte_ptr = reinterpret_cast<char*>(&offset);
  for (size_t i = 0; i < sizeof(uint64_t); i++) {
    std::cout << std::hex << (int)byte_ptr[i] << " ";
  }
  std::cout << std::endl;
}

TEST(FormatTest, StringPointer) {
  std::string s = "abcd";
  std::string* p = &s;
  std::cout << p << std::endl;
  s.append("efgh");
  std::cout << p << std::endl;
}

TEST(FormatTest, FooterManipulate) {
  BlockHandle metaindex_handle, index_handle;
  metaindex_handle.set_offset(1234);
  metaindex_handle.set_size(120);
  index_handle.set_offset(5678);
  index_handle.set_size(230);
  Footer footer;
  footer.set_index_handle(index_handle);
  footer.set_metaindex_handle(metaindex_handle);

  std::string s;
  footer.EncodeTo(&s);
  ASSERT_EQ(s.size(), Footer::kEncodedLength);
} 
}