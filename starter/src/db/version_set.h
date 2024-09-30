// The representation of a DBImpl consists of a set of Versions.  The
// newest version is called "current".  Older versions may be kept
// around to provide a consistent view to live iterators.
//
// Each Version keeps track of a set of Table files per level.  The
// entire set of versions is maintained in a VersionSet.
//
// Version,VersionSet are thread-compatible, but require external
// synchronization on all accesses.

#ifndef MINILSM_DB_VERSION_SET_H_
#define MINILSM_DB_VERSION_SET_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "db/dbformat.h"
// #include "db/version_edit.h"
#include "minilsm/options.h"
#include "port/port.h"

namespace minilsm {

class Iterator;
class MemTable;
class Version;
class VersionSet;

class Version {
public:
  // Reference count management (so Versions do not disappear out from
  // under live iterators)
  void Ref();
  void Unref();
private:
  friend class VersionSet;
  explicit Version(VersionSet* vset)
      : vset_(vset),
        next_(this),
        prev_(this),
        refs_(0) {}

  Version(const Version&) = delete;
  Version& operator=(const Version&) = delete;

  VersionSet* vset_;  // VersionSet to which this Versions belongs
  Version* next_;  // next version in linked list
  Version* prev_;  // previous version in linked list
  int refs_;  // Number of live references to this Version.
};

class VersionSet {
public:
  VersionSet(const std::string& dbname, const Options* options);
  Version* current() const { return current_; }
  uint64_t LastSequence() const { return last_sequence_; }
  void SetLastSequence(uint64_t seq) { 
    // has to be ascending
    assert(seq >= last_sequence_);
    last_sequence_ = seq; 
  }
private:
  friend class Version;

  void AppendVersion(Version* v);
  
  const std::string dbname_;
  const Options* const options_;

  uint64_t last_sequence_;

  Version dummy_versions_;  // Head of circular doubly-linked list of versions
  Version* current_;  // == dummy_versions_.prev_
};

}

#endif  // MINILSM_DB_VERSION_SET_H_