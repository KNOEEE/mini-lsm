#include "db/version_set.h"

#include <stdio.h>

#include <algorithm>

#include "db/memtable.h"
#include "util/coding.h"
#include "util/logging.h"

namespace minilsm {

void Version::Ref() { ++refs_; }

void Version::Unref() {
  assert(this != &vset_->dummy_versions_);
  assert(refs_ >= 1);
  --refs_;
  if (refs_ == 0) {
    delete this;
  }
}

VersionSet::VersionSet(const std::string& dbname, const Options* options)
    : dbname_(dbname),
      options_(options),
      dummy_versions_(this),
      current_(nullptr) {
  AppendVersion(new Version(this));
}

void VersionSet::AppendVersion(Version* v) {
  // Make v current
  assert(v->refs_ == 0);
  assert(v != current_);
  if (current_ != nullptr) {
    current_->Unref();
  }
  current_ = v;
  v->Ref();
  // Append to linked list
  v->prev_ = dummy_versions_.prev_;
  v->next_ = &dummy_versions_;
  v->prev_->next_ = v;
  v->next_->prev_ = v;
}

}  // namespace minilsm