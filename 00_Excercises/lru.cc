//
// A Simple Implementation of LRU Cache
//
// Hao Wu, 05/12/2014
//
// Reference 1: http://hawstein.com/posts/lru-cache-impl.html
// Reference 2: http://developer.51cto.com/art/200907/138772.htm
//
// Note:
//   In Put/Get methods of class Cache, to find a key in KeyEntryMap
//   we can write the following code:
//     >> typename KeyEntryMap::iterator it = key_entry_map_.find(key);
//   The reason of add 'typename':
//     http://goo.gl/LPSJQI.
//   Or, in C++11 we can also use:
//     >> auto it = key_entry_map_.find(key);
//   In addition, the following link provides discussions on how to find
//   a key in a hash map efficiently:
//     http://goo.gl/t5VYXW
//

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
//
using namespace std;

//
///
//

template<class K, class V>
struct CacheEntry
{
  K key;
  V value;
  CacheEntry* prev;
  CacheEntry* next;
};

//

template<class K, class V>
class Cache
{
public:
  Cache(int capacity);
  ~Cache();
  void Put(const K& key, const V& value);
  void Get(const K& key, V** pp_value);
private:
  typedef CacheEntry<K, V> Entry;
  typedef unordered_map<K, Entry*> KeyEntryMap;
  void Attach(Entry* p);
  void Detach(Entry* p);
  Entry*         head_;
  Entry*         tail_;
  vector<Entry>  z_entry_;
  vector<Entry*> free_entry_list_;
  KeyEntryMap    key_entry_map_;
};

//
///
//

template<class K, class V>
Cache<K, V>::Cache(int capacity)
  : z_entry_(capacity)
{
  for (int i = 0; i < (int)z_entry_.size(); i++) {
    free_entry_list_.push_back(&(z_entry_[i]));
  }
  head_ = new Entry();
  tail_ = new Entry();
  head_->prev = NULL;
  head_->next = tail_;
  tail_->prev = head_;
  tail_->prev = NULL;
}

//

template<class K, class V>
Cache<K, V>::~Cache()
{
  delete head_;
  delete tail_;
}

//

template<class K, class V>
void Cache<K, V>::Put(const K& key, const V& value)
{
  Entry* p = key_entry_map_[key];
  if (p != NULL) {
    p->value = value;
    Detach(p);
    Attach(p);
  } else {
    if (free_entry_list_.empty()) {
      Entry* q = tail_->prev;
      Detach(q);
      free_entry_list_.push_back(q);
      key_entry_map_.erase(q->key);
    }
    p = free_entry_list_.back();
    p->key = key;
    p->value = value;
    Attach(p);
    free_entry_list_.pop_back();
    key_entry_map_[key] = p;
  }
}

//

template<class K, class V>
void Cache<K, V>::Get(const K& key, V** pp_value)
{
  Entry* p = key_entry_map_[key];
  if (p != NULL) {
    *pp_value = &(p->value);
    Detach(p);
    Attach(p);
  } else {
    *pp_value = NULL;
  }
}

//

template<class K, class V>
void Cache<K, V>::Attach(Entry* p)
{
  p->prev = head_;
  p->next = head_->next;
  head_->next->prev = p;
  head_->next = p;
}

//

template<class K, class V>
void Cache<K, V>::Detach(Entry* p)
{
  p->prev->next = p->next;
  p->next->prev = p->prev;
}

//
///
//

int main()
{
  string* p_s;
  Cache<int, string> cache(3);
  cache.Put(1, "abc");
  cache.Put(2, "def");
  cache.Get(1, &p_s);
  cache.Put(3, "ghi");
  cache.Put(4, "xyz");
  cache.Put(3, "ghijk");
  cache.Get(1, &p_s);
  cache.Get(1, &p_s);
  cache.Get(1, &p_s);
  cache.Get(3, &p_s);
  if (p_s == NULL) {
    cout << "Cache miss." << endl;
  } else {
    cout << "Cache hit: " << *p_s << endl;
  }
  return 0;
}

//
///
////
///
//

