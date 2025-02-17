#ifndef MINILSM_DB_SKIPLIST_H_
#define MINILSM_DB_SKIPLIST_H_
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <iostream> // for Print
#include <string>   // for Print
#include <unordered_map>  // // for Print
#include <vector>   // for Print

#include "util/arena.h"
#include "util/random.h"
/**
 * @brief This Repo is basically from leveldb
 */
namespace minilsm {

template <typename Key, class Comparator>
class SkipList {
private:
  struct Node;
public:
  explicit SkipList(Comparator cmp, Arena* arena);
  SkipList(const SkipList&) = delete;
  SkipList& operator=(const SkipList&) = delete;

  // Requires: nothing that compares equal to key is in the list
  void Insert(const Key& key);

  bool Contains(const Key& key) const;
  void Print() const;

  class Iterator {
  public:
    // the returned iterator is invalid
    explicit Iterator(const SkipList* list);  

    bool Valid() const;
    const Key& key() const;
    void Next();
    void Prev();
    // Advance to the first entry with a key >= target
    void Seek(const Key& target);
    // Position at the first entry in list.
    // Final state of iterator is Valid() iff list is not empty.
    void SeekToFirst();

    // Position at the last entry in list.
    // Final state of iterator is Valid() iff list is not empty.
    void SeekToLast();

   private:
    const SkipList* list_;
    Node* node_;
    // Intentionally copyable
  };

private:
  enum { kMaxHeight = 12 };
  inline int GetMaxHeight() const {
    return max_height_.load(std::memory_order_relaxed);
  }
  Node* NewNode(const Key& key, int height);
  int RandomHeight();
  bool Equal(const Key& a, const Key& b) const { return (compare_(a, b) == 0); }
  // Return true if key is greater than the data stored in "n"
  bool KeyIsAfterNode(const Key& key, Node* n) const;

  // Return the earliest node that comes at or after key.
  // Return nullptr if there is no such node.
  //
  // If prev is non-null, fills prev[level] with pointer to previous
  // node at "level" for every level in [0..max_height_-1].
  Node* FindGreaterOrEqual(const Key& key, Node** prev) const;

  // Return the latest node with a key < key.
  // Return head_ if there is no such node.
  Node* FindLessThan(const Key& key) const;

  // Return the last node in the list.
  // Return head_ if list is empty.
  Node* FindLast() const;
  Comparator const compare_;  // Immutable after construction
  Arena* const arena_;
  Node* const head_;

  std::atomic<int> max_height_;
  Random rnd_;  // RW only by Insert()
};
  
template <typename Key, class Comparator>
struct SkipList<Key, Comparator>::Node {
  explicit Node(const Key& k) : key(k) {}
  Key const key;
  Node* Next(int n) {
    assert(n >= 0);
    return next_[n].load(std::memory_order_acquire);
  }
  void SetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].store(x, std::memory_order_release);
  }

  Node* NoBarrier_Next(int n) {
    assert(n >= 0);
    return next_[n].load(std::memory_order_relaxed);
  }
  void NoBarrier_SetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].store(x, std::memory_order_relaxed);
  }
private:
  // size is equal to the node height. next_[0] is the lowest level link
  std::atomic<Node*> next_[1];
};

template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node* SkipList<Key, Comparator>::NewNode(
    const Key& key, int height) {
  // number of Node* should be equal to height, but sizeof(node) has
  // includeed one Node*, so height - 1 here
  char* const node_memory = arena_->AllocateAligned(
      sizeof(Node) + sizeof(std::atomic<Node*>) * (height - 1));
  return new (node_memory) Node(key);
}

template <typename Key, class Comparator>
inline SkipList<Key, Comparator>::Iterator::Iterator(const SkipList* list) {
  list_ = list;
  node_ = nullptr;
}

template <typename Key, class Comparator>
inline bool SkipList<Key, Comparator>::Iterator::Valid() const {
  return node_ != nullptr;
}

template <typename Key, class Comparator>
inline const Key& SkipList<Key, Comparator>::Iterator::key() const {
  assert(Valid());
  return node_->key;
}

template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::Next() {
  assert(Valid());
  node_ = node_->Next(0);
}

// implemented by FindLess
template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::Prev() {
  assert(Valid());
  node_ = list_->FindLessThan(node_->key);
  if (node_ == list_->head_) 
    node_ = nullptr;
}

template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::Seek(const Key& target) {
  node_ = list_->FindGreaterOrEqual(target, nullptr);
}

template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::SeekToFirst() {
  node_ = list_->head_->Next(0);
}

template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::SeekToLast() {
  node_ = list_->FindLast();
  if (node_ == list_->head_) {
    node_ = nullptr;
  }
}

template <typename Key, class Comparator>
int SkipList<Key, Comparator>::RandomHeight() {
  static const unsigned int kBranching = 4;
  // probability = 1 in 4
  int height = 1;
  while (height < kMaxHeight && rnd_.OneIn(kBranching)) {
    height++;
  }
  assert(height > 0);
  assert(height <= kMaxHeight);
  return height;
}

template <typename Key, class Comparator>
bool SkipList<Key, Comparator>::KeyIsAfterNode(const Key& key, Node* n) const {
  return (n != nullptr) && (compare_(n->key, key) < 0);
}

template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node*
SkipList<Key, Comparator>::FindGreaterOrEqual(const Key& key,
                                              Node** prev) const {
  Node* x = head_;
  int level = GetMaxHeight() - 1;
  while (true) {
    Node* next = x->Next(level);  // find hight level first
    if (KeyIsAfterNode(key, next)) {
      x = next;
    } else {
      if (prev != nullptr) prev[level] = x;
      if (level == 0) {
        return next;
      } else {
        level--;
      }
    }
  }
}

// reverse logic of FindGreater
template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node*
SkipList<Key, Comparator>::FindLessThan(const Key& key) const {
  Node* x = head_;
  int level = GetMaxHeight() - 1;
  while (true) {
    assert(x == head_ || compare_(x->key, key) < 0);
    Node* next = x->Next(level);
    if (next == nullptr || compare_(next->key, key) >= 0) {
      if (level == 0) {
        return x;
      } else {
        level--;
      }
    } else {
      x = next;
    }
  }
}

template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node* SkipList<Key, Comparator>::FindLast()
    const {
  Node* x = head_;
  int level = GetMaxHeight() - 1;
  while (true) {
    Node* next = x->Next(level);
    if (next != nullptr) {
      x = next;
    } else {
      if (level == 0) {
        return x;
      } else {
        level--;
      }
    }
  }
}

template <typename Key, class Comparator>
SkipList<Key, Comparator>::SkipList(Comparator cmp, Arena* arena)
    : compare_(cmp),
      arena_(arena),
      head_(NewNode(0 /* any key will do*/, kMaxHeight)),
      max_height_(1),
      rnd_(0xdeadbeef) {
  for (int i = 0; i < kMaxHeight; i++) {
    head_->SetNext(i, nullptr);
  }
}

template <typename Key, class Comparator>
void SkipList<Key, Comparator>::Insert(const Key& key) {
  Node* prev[kMaxHeight];
  Node* x = FindGreaterOrEqual(key, prev);
  // do not allow duplicate insertion
  assert(x == nullptr || !Equal(key, x->key));
  int height = RandomHeight();
  if (height > GetMaxHeight()) {
    for (int i = GetMaxHeight(); i < height; i++) {
      prev[i] = head_;
    }
    // consider why we can set new height before set new node
    max_height_.store(height, std::memory_order_relaxed);
  }
  x = NewNode(key, height);
  for (int i = 0; i < height; i++) {
    x->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
    prev[i]->SetNext(i, x);
  }
}

template <typename Key, class Comparator>
bool SkipList<Key, Comparator>::Contains(const Key& key) const {
  Node* x = FindGreaterOrEqual(key, nullptr);
  if (x == nullptr) {
    return false;
  }
  return Equal(x->key, key);
}

template <typename Key, class Comparator>
void SkipList<Key, Comparator>::Print() const {
  Node* x = head_->Next(0);
  if (x == nullptr) {
    std::cout << "Skiplist is empty.\n";
    return;
  }
  // Here we better to check if Key is printable.
  int row_size = 2 * GetMaxHeight() - 1;
  std::vector<std::vector<std::string>> paint(row_size, 
                                              std::vector<std::string>());
  std::unordered_map<Key, size_t> location;
  // the lowest row
  size_t index = 0;
  while (true) {
    paint[row_size - 1].emplace_back(std::to_string(x->key));
    location[x->key] = index;
    index += std::to_string(x->key).size() + 1;
    x = x->Next(0);
    if (x == nullptr) {
      break;
    }
    paint[row_size - 1].emplace_back("-");
  }
  for (int row = row_size - 3; row >= 0; row -= 2) {
    // 2 * height + row = row_size - 1
    int height = (row_size - 1 - row) / 2;
    x = head_->Next(height);
    size_t next_key_index = location[x->key];
    index = 0;
    while (true) {
      std::string number_str = std::to_string(x->key);
      if (index < next_key_index) {
        paint[row].emplace_back("-");
        paint[row + 1].emplace_back(" ");
        index++;
      } else {
        paint[row].emplace_back(number_str);
        paint[row + 1].emplace_back("|");
        for (size_t i = 0; i < number_str.size() - 1; i++) {
          paint[row + 1].emplace_back(" ");
        }
        x = x->Next(height);
        if (x == nullptr) {
          break;
        }
        next_key_index = location[x->key];
        index += number_str.size();
      }
    }
  }
  for (int i = 0; i < row_size; i++) {
    for (size_t j = 0; j < paint[i].size(); j++) {
      std::cout << paint[i][j];
    }
    std::cout << std::endl;
  }
}
}  // namespace minilsm

#endif  // MINILSM_DB_SKIPLIST_H_
