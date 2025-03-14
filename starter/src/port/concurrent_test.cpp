#include "port/port.h"

#include <sys/time.h>

#include <iostream>
#include <thread>

#include <gtest/gtest.h>

namespace minilsm {

class ConcurrentTest : public ::testing::Test {
 public:
  void SetUp() override {
    flag = false;
  }
  static void Produce(int32_t wait_millis, bool notify = true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_millis));
    {
      // It should free the lock when notifying
      // because consumer wants to relock after waiting.
      std::unique_lock<std::mutex> lock(mutex_);
      flag = true;
    }
    if (notify)
      cv_.notify_one();
  }
  static void Consume(int32_t wait_millis,
                      int32_t exec_millis, bool log_timer = true) {
    Timer timer(log_timer);
    std::unique_lock<std::mutex> lock(mutex_);
    while (!flag) {
      if (cv_.wait_for(lock, std::chrono::milliseconds(wait_millis),
                       [&]() { return flag; })) {
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(exec_millis));
    }
  }
  static uint64_t milliseconds;

 private:
  class Timer {
   public:
    Timer(bool log = false) : start_millis_(NowMillis()), log_(log) {}
    ~Timer() {
      milliseconds = NowMillis() - start_millis_;
      if (log_)
        std::cout << "Time cost " << milliseconds << " milliseconds\n";
    }
   private:
    uint64_t NowMillis() {
      static constexpr uint64_t kMsecondsPerSecond = 1000;
      struct ::timeval tv;
      ::gettimeofday(&tv, nullptr);
      return static_cast<uint64_t>(tv.tv_sec) * kMsecondsPerSecond +
             tv.tv_usec / kMsecondsPerSecond;
    }
    uint64_t start_millis_ = 0;
    bool log_ = false;
  };
  static std::mutex mutex_;
  static std::condition_variable cv_;
  static bool flag;
};

std::mutex ConcurrentTest::mutex_;
std::condition_variable ConcurrentTest::cv_;
bool ConcurrentTest::flag;
uint64_t ConcurrentTest::milliseconds = 0;


TEST_F(ConcurrentTest, Base) {
  std::thread consumer(ConcurrentTest::Consume, 5, 10, false);
  std::thread producer(ConcurrentTest::Produce, 2, true);
  producer.join();
  consumer.join();
  ASSERT_EQ(ConcurrentTest::milliseconds, 2);
}

TEST_F(ConcurrentTest, EarlyProduce) {
  std::thread consumer(ConcurrentTest::Consume, 5, 10, false);
  std::thread producer(ConcurrentTest::Produce, 10, true);
  producer.join();
  consumer.join();
  // Consumer exits with no waiting at 2nd loop.
  ASSERT_EQ(ConcurrentTest::milliseconds, 15);
}

// Test how producer can wake up consumer with only flag or cv
// It can prove flag cannot wake up consumer at once
TEST_F(ConcurrentTest, DelayWakeup) {
  std::thread consumer(ConcurrentTest::Consume, 5, 5, false);
  std::thread producer(ConcurrentTest::Produce, 2, false);
  producer.join();
  consumer.join();
  ASSERT_EQ(ConcurrentTest::milliseconds, 5);
}
}  // namespace minilsm
