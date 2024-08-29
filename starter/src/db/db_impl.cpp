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
#include "db/version_set.h"
#include "db/write_batch_internal.h"
#include "minilsm/db.h"
#include "minilsm/status.h"
#include "util/coding.h"
#include "util/logging.h"
#include "util/mutexlock.h"

namespace minilsm {

const int kNumNonTableCacheFiles = 10;

// Information for every waiting writer
struct DBImpl::Writer {
  explicit Writer(port::Mutex* mu) 
      : batch(nullptr), sync(false), done(false), cv(mu) {}
  Status status;
  WriteBatch* batch;
  bool done;
  bool sync;
  port::CondVar cv;
};

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
      has_imm_(false),
      versions_(new VersionSet(dbname_, &options_)) {}

DBImpl::~DBImpl() {
  // Wait for background work to finish
  mutex_.Lock();
  shutting_down_.store(true, std::memory_order_release);
  mutex_.Unlock();

  delete versions_;
  if (mem_ != nullptr) mem_->Unref();
  if (imm_ != nullptr) imm_->Unref();
  delete tmp_batch_;
}

Status DBImpl::NewDB() {
  Status s;
  return s;
}

Status DBImpl::Recover(/* params */) {
  mutex_.AssertHeld();
  Status s;
  SequenceNumber max_sequence(0);
  if (versions_->LastSequence() < max_sequence) {
    versions_->SetLastSequence(max_sequence);
  }
  return s;
}

Status DBImpl::Get(const ReadOptions& options, const Slice& key,
                   std::string* value) {
  Status s;
  MutexLock l(&mutex_);
  // default use the last sequence
  SequenceNumber snapshot = versions_->LastSequence();

  MemTable* mem = mem_;
  MemTable* imm = imm_;
  Version* current = versions_->current();
  mem->Ref();
  if (imm != nullptr) imm->Ref();
  current->Ref();

  // Unlock while reading from files and memtables
  {
    mutex_.Unlock();
    // First look in the memtable, then in the immutable memtable (if any).
    LookupKey lkey(key, snapshot);
    if (mem->Get(lkey, value, &s)) {
      // Found
    } else if (imm != nullptr && imm->Get(lkey, value, &s)) {
      // Found
    } else {
      // Not found
      s = Status::NotFound(Slice());
    }
    // todo: skip sst for now
    mutex_.Lock();
  }
  mem->Unref();
  if (imm != nullptr) imm->Unref();
  current->Unref();
  return s;
}

Status DBImpl::Put(const WriteOptions& options, const Slice& key,
                    const Slice& value) {
  return DB::Put(options, key, value);
}

Status DBImpl::Delete(const WriteOptions& options, const Slice& key) {
  return DB::Delete(options, key);
}

Status DBImpl::Write(const WriteOptions& options, WriteBatch* updates) {
  Writer w(&mutex_);
  w.batch = updates;
  w.sync = options.sync;
  w.done = false;

  MutexLock l(&mutex_);  // auto unlock when destruction
  writers_.push_back(&w);
  while (!w.done && &w != writers_.front()) {
    w.cv.Wait();
  }
  // w might have been done by previous writer
  if (w.done) {
    return w.status;
  }

  // Write to memtable
  Status status;
  uint64_t last_sequence = versions_->LastSequence();
  Writer* last_writer = &w;
  if (status.ok() && updates != nullptr) { // nullptr batch is for compactions
    WriteBatch* write_batch = BuildBatchGroup(&last_writer);
    // updates must be the first in deque
    // increasing
    WriteBatchInternal::SetSequence(write_batch, last_sequence + 1);  
    // number of records
    last_sequence += WriteBatchInternal::Count(write_batch);  

    // apply to memtable. We can release the lock
    // during this phase since &w is currently responsible for protecting
    // against concurrent writes into mem_.
    {
      mutex_.Unlock();
      status = WriteBatchInternal::InsertInto(write_batch, mem_);
      mutex_.Lock();
    }
    if (write_batch == tmp_batch_) tmp_batch_->Clear();
    versions_->SetLastSequence(last_sequence);  // last operation number
  }
  while (true) {
    Writer* ready = writers_.front();
    writers_.pop_front();
    if (ready != &w) {
      ready->status = status;
      ready->done = true;
      ready->cv.Signal();
    } 
    if (ready == last_writer) break;
  }
  // Notify new head of write queue
  if (!writers_.empty()) {
    writers_.front()->cv.Signal();
  }
  return status;
}

// REQUIRE: writer list must be non-empty
// REQUIRE: first writer must have a non-null batch
WriteBatch* DBImpl::BuildBatchGroup(Writer** last_writer) {
  mutex_.AssertHeld();
  assert(!writers_.empty());
  // merge the current batch into a larger batch
  Writer* first = writers_.front();  
  WriteBatch* result = first->batch;
  assert(result != nullptr);

  size_t size = WriteBatchInternal::ByteSize(result);
  // Allow the group to grow up to a maximum size, but if the 
  // original write is small, limit the growth so we do not slow
  // down the small write too much.
  size_t max_size = 1 << 20;  // 1MB
  if (size <= (128 << 10)) {  // 128KB
    max_size = size += (128 << 10);
  }

  *last_writer = first;  // the last writer in the batch group
  // advance past "first"
  std::deque<Writer*>::iterator iter = writers_.begin() + 1;  

  for (; iter != writers_.end(); ++iter) {
    Writer* w = *iter;
    if (w->sync && !first->sync) {
      // Do not include a sync write into a batch handled by a non-sync write.
      break;
    }
    if (w->batch != nullptr) {
      size += WriteBatchInternal::ByteSize(w->batch);
      if (size > max_size) {
        // Do not make batch too big
        break;
      }
      // Append to *result
      if (result == first->batch) {
        // Switch to temporary batch instead of disturbing caller's batch
        result = tmp_batch_;
        assert(WriteBatchInternal::Count(result) == 0);
        WriteBatchInternal::Append(result, first->batch);
      }
      WriteBatchInternal::Append(result, w->batch);
    }
    *last_writer = w;
  }
  return result;
}

// Default implementations of convenience methods that subclasses of DB
// can call if they wish
Status DB::Put(const WriteOptions& opt, const Slice& key, const Slice& value) {
  WriteBatch batch;
  batch.Put(key, value);
  return Write(opt, &batch);
}

Status DB::Delete(const WriteOptions& opt, const Slice& key) {
  WriteBatch batch;
  batch.Delete(key);
  return Write(opt, &batch);
}

Status DB::Open(const Options& options, const std::string& dbname, DB** dbptr) {
  *dbptr = nullptr;
  DBImpl* impl = new DBImpl(options, dbname);
  impl->mutex_.Lock();
  Status s = impl->Recover();
  if (s.ok() && impl->mem_ == nullptr) {
    impl->mem_ = new MemTable(impl->internal_comparator_);
    impl->mem_->Ref();
  }
  impl->mutex_.Unlock();
  if (s.ok()) {
    assert(impl->mem_ != nullptr);
    *dbptr = impl;
  }
  return s;
}
DB::~DB() = default;

}  // namespace minilsm