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

}

}  // namespace minilsm