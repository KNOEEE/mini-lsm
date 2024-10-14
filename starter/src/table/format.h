// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef MINILSM_TABLE_FORMAT_H_
#define MINILSM_TABLE_FORMAT_H_

#include <stdint.h>

#include <string>

#include "minilsm/slice.h"
#include "minilsm/status.h"
// #include "minilsm/table_builder.h"

namespace minilsm {

class Block;
struct ReadOptions;

struct BlockContents {
  Slice data;           // Actual contents of data
  bool cachable;        // True iff data can be cached
  bool heap_allocated;  // True iff caller should delete[] data.data()
};

}

#endif  // MINILSM_TABLE_FORMAT_H_