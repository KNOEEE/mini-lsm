// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef MINILSM_TABLE_BLOCK_H_
#define MINILSM_TABLE_BLOCK_H_

#include <stddef.h>
#include <stdint.h>

#include "minilsm/iterator.h"

namespace minilsm {

struct BlockContents;
class Comparator;

// https://github.com/google/leveldb/blob/main/doc/table_format.md
class Block {
 public:
  // Initialize the block with the specified contents.
  explicit Block(const BlockContents& contents);

  Block(const Block&) = delete;
  Block& operator=(const Block&) = delete;

  ~Block();

  size_t size() const { return size_; }
  Iterator* NewIterator(const Comparator* comparator);

 private:
  class Iter;

  uint32_t NumRestarts() const;

  const char* data_;
  size_t size_;
  uint32_t restart_offset_;  // Offset in data_ of restart array
  bool owned_;               // Block owns data_[]
};

}  // namespace minilsm

#endif  // MINILSM_TABLE_BLOCK_H_
