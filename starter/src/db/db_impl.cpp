#include "db/db_impl.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <set>
#include <string>
#include <vector>

#include "db/dbformat.h"
#include "db/memtable.h"
#include "minilsm/db.h"
#include "minilsm/status.h"
#include "util/coding.h"
#include "util/logging.h"

namespace minilsm {

const int kNumNonTableCacheFiles = 10;

template <class T, class V>
static void ClipToRange(T* ptr, V minvalue, V maxvalue) {
  if (static_cast<V>(*ptr) > maxvalue) *ptr = maxvalue;
  if (static_cast<V>(*ptr) < minvalue) *ptr = minvalue;
}

Options SanitizeOptions(const std::string& db,
                        const InternalKeyComparator* icmp,
                        const Options& src) {
  Options result = src;
  result.comparator = icmp;
  ClipToRange(&result.max_open_files, 64 + kNumNonTableCacheFiles, 50000);
  ClipToRange(&result.write_buffer_size, 64 << 10, 1 << 30);
  ClipToRange(&result.max_file_size, 1 << 20, 1 << 30);
  ClipToRange(&result.block_size, 1 << 10, 4 << 20);
  return result;
}

DBImpl::DBImpl(const Options& raw_options, const std::string& dbname)
    : internal_comparator_(raw_options.comparator),
      options_(SanitizeOptions(dbname, &internal_comparator_, raw_options)),
      dbname_(dbname),
      shutting_down_(false),
      background_work_finished_signal_(&mutex_),
      mem_(nullptr),
      imm_(nullptr),
      has_imm_(false) {}

DBImpl::~DBImpl() {
  // Wait for background work to finish
  mutex_.Lock();
  shutting_down_.store(true, std::memory_order_release);
  mutex_.Unlock();

  if (mem_ != nullptr) mem_->Unref();
  if (imm_ != nullptr) imm_->Unref();
}

Status DBImpl::Get(const ReadOptions& options, const Slice& key,
                   std::string* value) {
  Status s;
  return s;
}
}