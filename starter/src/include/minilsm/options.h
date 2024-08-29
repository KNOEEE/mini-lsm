#ifndef MINILSM_INCLUDE_OPTIONS_H_
#define MINILSM_INCLUDE_OPTIONS_H_

#include <cstddef>

namespace minilsm {

class Comparator;

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

// passed to DB::Open
struct Options {
  Options();

  // Default: uses lexicographic byte-wise ordering
  const Comparator* comparator;
  bool create_if_missing = false;

  bool error_if_exists = false;
  bool paranoid_checks = false;

  // to do
  // env
  // logger
  // cache
  // filter policy

  // Amount of data to build up in memory
  size_t write_buffer_size = 4 * 1024 * 1024;
  int max_open_files = 1000;
  size_t block_size = 4 * 1024;

  // Number of keys between restart points for delta encoding of keys.
  int block_restart_interval = 16;
  size_t max_file_size = 2 * 1024 * 1024;
  CompressionType compression = kSnappyCompression;

  bool reuse_logs;
};

struct ReadOptions {
  bool verify_checksums = false;

  // Callers may wish to set this field to false for bulk scans
  bool fill_cache = true;

  // to do
  // snapshot
};

struct WriteOptions {
  WriteOptions() = default;

  // A DB write with sync==false has similar crash semantics as 
  // the "write()" systen call.
  // A DB write with sync==true has similar crash semantics to 
  // a "write()" system call followed by "fsync()"
  bool sync = false;
};

}
#endif  // MINILSM_INCLUDE_OPTIONS_H_