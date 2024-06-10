#include "util/logging.h"

#include <iostream>
#include <limits>
#include <string>

#include "minilsm/slice.h"

using namespace minilsm;

int main() {
  Slice s = "test-build";
  std::cout << EscapeString(s) << std::endl;
}