#ifndef MINILSM_PORT_PORT_STDCXX_H_
#define MINILSM_PORT_PORT_STDCXX_H_

#if HAVE_CRC32C
#include <crc32c/crc32c.h>
#endif  // HAVE_CRC32C
#if HAVE_SNAPPY
// 'snappy.h' file not found with <angled> include; use "quotes" instead (fix available)
#include "snappy.h"
#endif  // HAVE_SNAPPY

#include <cassert>
#include <condition_variable>  // NOLINT
#include <cstddef>
#include <cstdint>
#include <mutex>  // NOLINT
#include <string>

namespace minilsm {

namespace port {

class CondVar;

// Thinly wraps std::mutex
class Mutex {
public:
  Mutex() = default;
  ~Mutex() = default;

  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex&) = delete;

  void Lock() { mu_.lock(); }
  void Unlock() {mu_.unlock(); }
  void AssertHeld() {}
private:
  friend class CondVar;
  std::mutex mu_;
};

// Thinly wraps std::condition_variable.
class CondVar {
public:
  explicit CondVar(Mutex* mu) : mu_(mu) { assert(mu != nullptr); }
  ~CondVar() = default;

  CondVar(const CondVar&) = delete;
  CondVar& operator=(const CondVar&) = delete;

  void Wait() {
    // https://en.cppreference.com/w/cpp/thread/lock_tag_t
    std::unique_lock<std::mutex> lock(mu_->mu_, std::adopt_lock);
    cv_.wait(lock);
    lock.release();
  }
  void Signal() { cv_.notify_one(); }
  void SignalAll() {cv_.notify_all(); }
private:
  std::condition_variable cv_;
  // a const pointer, value of pointer (address) cannot be changed
  Mutex* const mu_;
};

inline bool Snappy_Compress(const char* input, size_t length,
                            std::string* output) {
#if HAVE_SNAPPY
  output->resize(snappy::MaxCompressedLength(length));
  size_t outlen;
  snappy::RawCompress(input, length, &(*output)[0], &outlen);
  output->resize(outlen);
  return true;
#else
  // Silence compiler warnings about unused arguments.
  (void)input;
  (void)length;
  (void)output;
#endif  // HAVE_SNAPPY

  return false;
}

inline bool Snappy_GetUncompressedLength(const char* input, size_t length,
                                         size_t* result) {
#if HAVE_SNAPPY
  return snappy::GetUncompressedLength(input, length, result);
#else
  // Silence compiler warnings about unused arguments.
  (void)input;
  (void)length;
  (void)result;
  return false;
#endif  // HAVE_SNAPPY
}

inline bool Snappy_Uncompress(const char* input, size_t length, char* output) {
#if HAVE_SNAPPY
  return snappy::RawUncompress(input, length, output);
#else
  // Silence compiler warnings about unused arguments.
  (void)input;
  (void)length;
  (void)output;
  return false;
#endif  // HAVE_SNAPPY
}

}  // namespace port
}  // namespace minilsm

#endif  // MINILSM_PORT_PORT_STDCXX_H_