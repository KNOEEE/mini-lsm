#include "minilsm/env.h"

namespace minilsm {
// <C++ Primer> zh P552 virtual destructor
Env::~Env() = default;

RandomAccessFile::~RandomAccessFile() = default;
}  // namespace minilsm