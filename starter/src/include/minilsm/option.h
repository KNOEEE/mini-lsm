#ifndef MINILSM_INCLUDE_OPTIONS_H_
#define MINILSM_INCLUDE_OPTIONS_H_

#include <cstddef>

namespace minilsm {

// DB contents are stored in a set of blocks, each of which holds a
// sequence of key,value pairs.  Each block may be compressed before
// being stored in a file.  The following enum describes which
// compression method (if any) is used to compress a block.
enum CompressionType {
  // NOTE: do not change the values of existing entries, as these are
  // part of the persistent format on disk.
  kNoCompression = 0x0,
  kSnappyCompression = 0x1
};

}
#endif  // MINILSM_INCLUDE_OPTIONS_H_