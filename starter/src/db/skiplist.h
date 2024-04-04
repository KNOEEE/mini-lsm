#ifndef MINILSM_DB_SKIPLIST_H_
#define MINILSM_DB_SKIPLIST_H_
#include <atomic>
#include <cassert>
#include <cstdlib>

#include "util/arena.h"
#include "util/random.h"
namespace minilsm {

template <typename Key, class Comparator>
class SkipList {
private:
  struct Node;
public:
  explicit SkipList(Comparator cmp, Arena* arena);
  SkipList(const SkipList&) = delete;
  SkipList& opreator=(const SkipList&) = delete;

  // Requires: nothing that compares equal to key is in the list
  void Insert(const Key& key);

  bool Contains(const Key& key) const;

  class Iterator {
  public:
    // the returned iterator is invalid
    explicit Iterator(const SkipList* list);  

    bool Valid() const;
    const Key& key() const;
    void Next();
    void Prev();
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
private:
  // size is equal to the node height. next_[0] is the lowest level link
  std::atomic<Node*> next_[1];
};
}  // namespace minilsm

#endif  // MINILSM_DB_SKIPLIST_H_
