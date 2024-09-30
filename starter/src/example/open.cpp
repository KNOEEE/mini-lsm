#include <iostream>
#include <vector>

#include "minilsm/db.h"
#include "minilsm/iterator.h"
#include "minilsm/status.h"

using namespace minilsm;

void TestIterator(DB* db_ptr) {
  minilsm::Iterator* iter = db_ptr->NewIterator(minilsm::ReadOptions());
  iter->SeekToFirst();
  while (iter->Valid()) {
    std::cout << iter->key().ToString() << " " << iter->value().ToString() << std::endl;
    iter->Next();
  }
  iter->SeekToLast();
  while (iter->Valid()) {
    std::cout << iter->key().ToString() << " " << iter->value().ToString() << std::endl;
    iter->Prev();
  }
}

int main() {
  minilsm::DB* db;
  minilsm::Options options;

  minilsm::Status status = minilsm::DB::Open(options,
                                             "~/db/minilsm", 
                                             &db);
  std::string key = "foo";
  std::string value = "abc";
  std::vector<std::string> key_vec = {"compute", "computer", "computes", 
                                      "computing"};
  for (int i = 0; i < 4; i++) {
    status = db->Put(minilsm::WriteOptions(), key_vec[i], value);
  }
  // status = db->Put(leveldb::WriteOptions(), key, value);

  std::string get_value;
  status = db->Get(minilsm::ReadOptions(), "compute", &get_value);

  if (status.ok()) {
    std::cout << "get value " << get_value << std::endl;
  } else {
    std::cout << "err during read\n";
  }
  TestIterator(db);

  delete db;
  return 0;
}