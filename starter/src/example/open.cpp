#include <iostream>
#include <vector>

#include "minilsm/db.h"
#include "minilsm/iterator.h"
#include "minilsm/status.h"


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

  delete db;
  return 0;
}