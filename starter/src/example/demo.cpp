
#include <iostream>
#include <limits>
#include <string>

#include "minilsm/slice.h"
#include "util/logging.h"
#include "util/random.h"

using namespace minilsm;

int main() {
  Slice s = "test-build";
  std::cout << EscapeString(s) << std::endl;

  Random rnd(0xabcd);
  std::cout << rnd.Next() << std::endl;
}