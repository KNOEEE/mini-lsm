#ifndef MINILSM_DB_DB_IMPL_H_
#define MINILSM_DB_DB_IMPL_H_
#include <atomic>
#include <deque>
#include <set>
#include <string>

#include "db/dbformat.h"
#include "minilsm/db.h"
#include "port/port.h"


namespace minilsm {

class MemTable;
class VersionSet;

class DBImpl : public DB {
public:
  DBImpl(const Options& options, const std::string& dbname);

  DBImpl(const DBImpl&) = delete;
  DBImpl& operator=(const DBImpl&) = delete;
  ~DBImpl() override;

  Status Put(const WriteOptions&, const Slice& key,
             const Slice& value) override;
  Status Delete(const WriteOptions&, const Slice& key) override;
  Status Write(const WriteOptions& options, WriteBatch* updates) override;
  Status Get(const ReadOptions& options, const Slice& key,
             std::string* value) override;

private:
  friend class DB;
  struct Writer;

  Status NewDB();
  // Recover the descriptor from persistent storage.  May do a significant
  // amount of work to recover recently logged updates.  Any changes to
  // be made to the descriptor are added to *edit.
  Status Recover();

  WriteBatch* BuildBatchGroup(Writer** last_writer);

  // For comparing internal keys, but compare userkey with a user comparator
  const InternalKeyComparator internal_comparator_;
  const Options options_;
  const std::string dbname_;

  // State below is protected by mutex_
  port::Mutex mutex_;
  std::atomic<bool> shutting_down_;
  port::CondVar background_work_finished_signal_;
  MemTable* mem_;
  MemTable* imm_;
  std::atomic<bool> has_imm_;  // So bg thread can detect non-null imm_

  // Queue of writers.
  std::deque<Writer*> writers_;
  WriteBatch* tmp_batch_;

  VersionSet* const versions_;
};

Options SanitizeOptions(const std::string& db,
                        const InternalKeyComparator* icmp,
                        const Options& src);
}
#endif  // MINILSM_DB_MEMTABLE_H_
