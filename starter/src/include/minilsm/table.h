#ifndef MINILSM_INCLUDE_TABLE_H_
#define MINILSM_INCLUDE_TABLE_H_

#include <stdint.h>
#include "minilsm/iterator.h"
/**
 * @Chpater 1.4 One has to finish block before implementing this class.
 * https://skyzh.github.io/mini-lsm/week1-04-sst.html
 */
namespace minilsm {

class Block;
struct Options;
class RandomAccessFile;
struct ReadOptions;

// A Table is a sorted map from strings to strings.  Tables are
// immutable and persistent.  A Table may be safely accessed from
// multiple threads without external synchronization.
class Table {
 public:
  // Attempt to open the table that is stored in bytes [0..file_size)
  // of "file", and read the metadata entries necessary to allow
  // retrieving data from the table.
  //
  // If successful, returns ok and sets "*table" to the newly opened
  // table.  The client should delete "*table" when no longer needed.
  // If there was an error while initializing the table, sets "*table"
  // to nullptr and returns a non-ok status.  Does not take ownership of
  // "*source", but the client must ensure that "source" remains live
  // for the duration of the returned table's lifetime.
  //
  // *file must remain live while this Table is in use.
  static Status Open(const Options& options, RandomAccessFile* file,
                     uint64_t file_size, Table** table);

  Table(const Table&) = delete;
  Table& operator=(const Table&) = delete;

  ~Table();
 private:
  struct Rep;

  static Iterator* BlockReader(void*, const ReadOptions&, const Slice&);
  explicit Table(Rep* rep) : rep_(rep) {}
  Rep* const rep_;  // const pointer
};  // class Table
}  // namespace minilsm
#endif  // MINILSM_INCLUDE_TABLE_H_