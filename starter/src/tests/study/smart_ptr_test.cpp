#include <gtest/gtest.h>

namespace minilsm {

class SmartPointerTest : public ::testing::Test {};

class A {
 public:
  int num = 0;
};

void FuncPtr(std::unique_ptr<A> a_ptr) {
  std::cout << a_ptr->num << std::endl;
}

void FuncRef(const std::unique_ptr<A>& a_ptr) {
  std::cout << a_ptr->num << std::endl;
}

TEST_F(SmartPointerTest, Base) {
  std::unique_ptr<A> ptr = std::make_unique<A>();
  ptr->num = 100;
  FuncPtr(std::move(ptr));
  ASSERT_EQ(ptr, nullptr);
}

TEST_F(SmartPointerTest, Ref) {
  std::unique_ptr<A> ptr = std::make_unique<A>();
  ptr->num = 100;
  FuncRef(ptr);
  ASSERT_NE(ptr, nullptr);
}

}  // namespace minilsm