#include "minilsm/table.h"

// #include "minilsm/cache.h"
#include "minilsm/comparator.h"
#include "minilsm/env.h"
// #include "leveldb/filter_policy.h"
#include "minilsm/options.h"
#include "table/block.h"
// #include "table/filter_block.h"
#include "table/format.h"
// #include "table/two_level_iterator.h"
#include "util/coding.h"

namespace minilsm {

struct Table::Rep {
  ~Rep() {
    
  }
  Options options;
  Status status;
  RandomAccessFile* file;
  uint64_t cache_id;
  // todo: Filter not implemented yet
  BlockHandle metaindex_handle;  // Handle to metaindex_block: saved from footer
  Block* index_block;
};

Status Table::Open(const Options &options, RandomAccessFile *file, 
                   uint64_t size, Table **table) {
  // 修改传入指针的指向
  *table = nullptr;
  if (size < Footer::kEncodedLength) {
    return Status::Corruption("file is too short to be an sstable");
  }
  char footer_space[Footer::kEncodedLength];
  Slice footer_input;
  Status s = file->Read(size - Footer::kEncodedLength, Footer::kEncodedLength, 
                        &footer_input, footer_space);
  if (!s.ok()) return s;

  Footer footer;
  s = footer.DecodeFrom(&footer_input);
  if (!s.ok()) return s;
  // Read the index block
  BlockContents index_block_contents;
  if (s.ok()) {
    ReadOptions opt;
    if (options.paranoid_checks) {
      opt.verify_checksums = true;
    }
    s = ReadBlock(file, opt, footer.index_handle(), &index_block_contents);
  }
  if (s.ok()) {
    // We've successfully read the footer and the index block: we're
    // ready to serve requests.
    Block* index_block = new Block(index_block_contents);
    Rep* rep = new Table::Rep;
    rep->options = options;
    rep->file = file;
    rep->metaindex_handle = footer.metaindex_handle();
    rep->index_block = index_block;
    // TODO implement cache
    // rep->cache_id = (options.block_)
  }
}
}  // namespace minilsm
