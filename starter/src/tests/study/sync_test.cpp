#include <iostream>
#include <mutex>
#include <thread>
#include <type_traits>

#include <gtest/gtest.h>

namespace minilsm {

// https://stackoverflow.com/questions/573294/when-to-use-reinterpret-cast
class SyncTest : public ::testing::Test {};

void CallOnce() {
  static std::once_flag flag;
  std::call_once(flag, []() {
    std::cout << "call_once start\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "call_once end\n";
  });
  std::cout << "CallOnce end\n";
}
TEST_F(SyncTest, BasicCall) {
  CallOnce();
}

TEST_F(SyncTest, NonBlock) {
  std::thread t1(CallOnce);
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::thread t2(CallOnce);
  // If a `std::thread` object that represents 
  // a thread of execution (i.e., it is joinable) is destroyed, 
  // the program is terminated by calling `std::terminate()`.
  t1.join();
  t2.join();
}

TEST_F(SyncTest, BlockWait) {
  std::thread t1(CallOnce);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::thread t2(CallOnce);
  t1.join();
  t2.join();
}

}  // namespace minilsm