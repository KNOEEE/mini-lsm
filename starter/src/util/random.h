#ifndef MINILSM_UTIL_RANDOM_H
#define MINILSM_UTIL_RANDOM_H

#include <cstdint>  // uint32_t

namespace minilsm {

class Random {
private:
  uint32_t seed_;
public:
  explicit Random(uint32_t s) : seed_(s & 0x7fffffffu) {
    if (seed_ == 0 || seed_ == 2147483647L) {  // max value of int32
      seed_ = 1;
    }
  }

  uint32_t Next() {
    static const uint32_t M = 2147483647L;  // 2^31 - 1
    static const uint64_t A = 16807;        // bits 14, 8, 7, 5, 2, 1, 0
    //       seed_ = (seed_ * A) % M,    where M = 2^31-1
    uint64_t product = seed_ * A;
    seed_ = static_cast<uint32_t>((product >> 31) + (product & M));
    // using > allows the faster sign-bit-based test.
    if (seed_ > M) {
      seed_ -= M;
    }
    return seed_;
  }

  uint32_t Uniform(int n) { return Next() % n; }
  // Randomly returns true ~"1/n" of the time
  bool OneIn(int n) {return (Next() % n) == 0; }
  uint32_t Skewed(int max_log) { return Uniform(1 << Uniform(max_log + 1)); }
};
}  // namespace minilsm

#endif  // MINILSM_UTIL_RANDOM_H
