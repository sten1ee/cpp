#ifndef UTIL_HASHTAB_H
#define UTIL_HASHTAB_H

#ifndef   UTIL_TYPES_H
#include "util/types.h"
#endif
#ifndef   UTIL_DEBUG_H
#include "util/debug.h"
#endif

#ifndef DEFAULT_HASHTABLE_SIZE
#define DEFAULT_HASHTABLE_SIZE 101
#endif
#ifndef DEFAULT_HASHTABLE_LOAD
#define DEFAULT_HASHTABLE_LOAD  75
#endif


template <class DATA> class HashTableTraits
{
/* Each specialization should contain following definitions:
  public:
    class   HashTableBase { ??? }; //you can make some functions virtual here

    typedef ????  Data;    //the data type stored in hashtable entries
    typedef ????  DataIn;  //the param type of function put
    typedef ????  DataOut; //the return type of function get
    typedef ????  Key;     //the param type of function get

    static  bool    Equals(Key k, DataIn d)
    static  hash_t  Hash(Key k)
    static  DataOut NullData()
    static  void    FlushData(Data& data, bool ownsData)
*/
};


template <class TRAITS> class HashTable : public TRAITS::HashTableBase
{
  protected:
    typedef TRAITS   Traits;
    typedef typename Traits::Data      Data;
    typedef typename Traits::DataIn    DataIn;
    typedef typename Traits::DataOut   DataOut;
    typedef typename Traits::Key       Key;

    struct Node
    {
      hash_t  hash;
      Data    data;
      Node*   next;

      Node(hash_t hash, DataIn data, Node* next)
        : hash(hash), data(data), next(next)
        {}
    };

    Node**    node;
    size_t    size;
    size_t    nodeCount;
    size_t    threshold;
    size_t    load;
    bool      owns;

    void  recalcThreshold()
      {
        threshold = size_t((load * (unsigned long)size) / 100);
        UTIL_ASSERT(nodeCount <= threshold);
      }

    void  init(size_t _size, size_t _load);

    void  copy(HashTable& ht)
      {
        node       = ht.node;
        size       = ht.size;
        nodeCount  = ht.nodeCount;
        threshold  = ht.threshold;
        load       = ht.load;
        owns       = ht.owns;
      }

    void  relinquish()
      {
        node     = 0;
        size     = 0;
        nodeCount= 0;
      }

  public:
   ~HashTable()
      {
        flush(ownsData());
        delete[] node;
      }

    HashTable(size_t size=DEFAULT_HASHTABLE_SIZE,
              size_t load=DEFAULT_HASHTABLE_LOAD)
      {
        init(size, load);
      }

    HashTable(HashTable& ht)
      {
        copy(ht);
        ht.relinquish();
      }

    HashTable&  operator = (HashTable& ht)
      {
        flush(ownsData());
        copy(ht);
        ht.relinquish();
        return *this;
      }

    bool    ownsData()     const { return owns;  }
    void    ownsData(bool _owns) { owns = _owns; }
    DataOut get(Key key, hash_t hash);
    DataOut get(Key key);
    void    put(DataIn data);
    bool    remove(Key key, bool delData);
    bool    remove(Key key)      { return remove(key, ownsData()); }
    void    flush(bool delData);
    void    flush()              { flush(ownsData()); }
    void    resizeTo(size_t new_size);
};

template <class TRAITS>
void  HashTable<TRAITS>::init(size_t _size, size_t _load)
  {
    size       = _size;
    node       = new Node*[size];
    for (size_t i=size; 0 < i--; ) {
      node[i] = 0;
    }
    nodeCount  = 0;
    load       = _load;
    owns       = true;
    recalcThreshold();
  }

template <class TRAITS>
HashTable<TRAITS>::DataOut  HashTable<TRAITS>::get(Key key)
  {
    return get(key, Traits::Hash(key));
  }

template <class TRAITS>
HashTable<TRAITS>::DataOut  HashTable<TRAITS>::get(Key key, hash_t hash)
  {
    for (Node* cur = node[size_t(hash % size)]; cur; cur = cur->next) {
      if (hash == cur->hash && Traits::Equals(key, cur->data)) {
        return cur->data;
      }
    }
    return Traits::NullData();
  }

template <class TRAITS>
void  HashTable<TRAITS>::put(DataIn data)
  {
    hash_t  hash = Traits::Hash(data);
    size_t  pos  = size_t(hash % size);
    node[pos]    = new Node(hash, data, node[pos]);
    if (++nodeCount > threshold) {
      UTIL_ASSERT(SIZET_MAX/2 > size);
      resizeTo(size * 2 + 1);
    }
  }

template <class TRAITS>
bool  HashTable<TRAITS>::remove(Key key, bool delData)
  {
    hash_t  hash = Traits::Hash(key);
    for (Node** ppn = &node[size_t(hash % size)];
                   *ppn;
                    ppn = &(*ppn)->next) {
      if (hash == (*ppn)->hash && Traits::Equals(key, (*ppn)->data)) {
        Traits::FlushData((*ppn)->data, delData);
        Node* pnRemoved = *ppn;
        *ppn = pnRemoved->next;
        delete pnRemoved;
        nodeCount--;
        return true;
      }
    }
    return false;
  }

template <class TRAITS>
void  HashTable<TRAITS>::flush(bool delData)
  {
    for (size_t i = size; 0 < i--; ) {
      Node* cur = node[i];
      node[i] = 0;
      while (cur) {
        Node* next = cur->next;
        Traits::FlushData(cur->data, delData);
        delete cur;
        cur = next;
      }
    }
    nodeCount = 0;
  }

template <class TRAITS>
void  HashTable<TRAITS>::resizeTo(size_t new_size)
  {
    UTIL_ASSERT(0 < new_size);
    Node** new_node = new Node*[new_size];
    for (size_t ni=new_size; 0 < ni--; ) {
      new_node[ni] = 0;
    }
    for (size_t i = size; 0 < i--; ) {
      Node* cur = node[i];
      while (cur) {
        Node* next = cur->next;
        size_t new_i  = size_t(cur->hash % new_size);
        cur->next = new_node[new_i];
        new_node[new_i] = cur;
        cur = next;
      }
    }
    delete[] node;
    node = new_node;
    size = new_size;
    recalcThreshold();
  }

#endif //UTIL_HASHTAB_H
