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
struct ReadOptions;

// A Table is a sorted map from strings to strings.  Tables are
// immutable and persistent.  A Table may be safely accessed from
// multiple threads without external synchronization.
class Table {
 public:
 private:
  struct Rep;

  static Iterator* BlockReader(void*, const ReadOptions&, const Slice&);
  explicit Table(Rep* rep) : rep_(rep) {}
  Rep* const rep_;  // const pointer
};  // class Table
}  // namespace minilsm
#endif  // MINILSM_INCLUDE_TABLE_H_