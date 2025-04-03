#include <iostream>
#include <type_traits>

#include <gtest/gtest.h>

namespace minilsm {

// https://stackoverflow.com/questions/573294/when-to-use-reinterpret-cast
class CastTest : public ::testing::Test {};

TEST_F(CastTest, StaticBasicType) {
  double a = 1.23;
  int b = static_cast<int>(a);
  ASSERT_EQ(b, 1);

  // Reinterpret_cast from 'double' to 'int' is not allowed
  // clang(bad_cxx_cast_generic)
  // int c = reinterpret_cast<int>(a);
}

TEST_F(CastTest, StaticDerived) {
  class Base {};
  class Derived : public Base {};
  Base* p = new Derived();
  Derived* pointer_to_derived = static_cast<Derived*>(p);
  if (!std::is_same<decltype(pointer_to_derived), Derived*>::value) {
    GTEST_FAIL();
  }
  // Convert with reinterpret_cast is dangerous.
}

TEST_F(CastTest, ReinterpretPointer) {
  int num = 0x12345678;
  // 将 int* 转换为 char*，按字节访问内存
  char* byte_ptr = reinterpret_cast<char*>(&num); 
  for (size_t i = 0; i < sizeof(int); i++) {
    std::cout << std::hex << (int)byte_ptr[i] << " "; // 输出各字节值（依赖字节序）
  }
  std::cout << std::endl;
}
}  // namespace minilsm