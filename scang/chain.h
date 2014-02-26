#ifndef CHAIN_H
#define CHAIN_H

#include "util/assert.h"

template <class Chain, typename Data>
class chain_iterator
{
  private:
  public:
    friend class Chain;
    typedef typename Chain::node  node;

    node* current;

    chain_iterator(node* c)
      : current(c)
      {;}

  public:
  
    bool  operator == (const chain_iterator& i) const
      {
        return current == i.current;
      }

    bool  operator != (const chain_iterator& i) const
      {
        return current != i.current;
      }
/*
    operator bool () const
      {
        return current != 0;
      }
*/
    Data& operator * () const
      {
        return current->data;
      }

    chain_iterator& operator ++ ()
      {
        current = current->next;
        return *this;
      }

    chain_iterator  operator ++ (int)
      {
        chain_iterator res(*this);
        current = current->next;
        return res;
      }
};


template <typename Data>
class chain
{
  public:
    struct node
    {
      Data  data;
      node* next;

      node(const Data& d, node* n)
        : data(d), next(n)
        {;}

     ~node()
        {
          if (next)
            delete next;
        }

      node* dup() const
        {
          return new node(data, next ? next->dup() : 0);
        }
    };

  private:
    node*  head;

    chain(const chain& c) {;}
    
  public:
    // next method is present as a work around to the
    // no meber templates problem in BC++ 5.02
    node*&  __head() { return head; }

    typedef chain_iterator<chain, Data>       iterator;
    typedef chain_iterator<chain, const Data> const_iterator;

    chain() : head(0) {;}

    chain(chain& c)
      {
        head = c.head;
        c.head = 0;
      }

   ~chain()  { flush(); }

    void  flush()
      {
        if (head)
          delete head;
      }

    void  flushPast(iterator i)
      {
        if (i.current && i.current->next) {
          delete i.current->next;
          i.current->next = 0;
        }
      }

    chain&  operator = (const chain& c)
      {
        flush();
        head = (c.head ? c.head->dup() : 0);
        return *this;
      }

    bool  operator == (const chain& c) const
      {
        return EqualContents(head, c.head);
      }

    bool  empty() const
      {
        return head == 0;
      }

    void  insert_front(const Data& data)
      {
        ASSERT(head == 0 || data < head->data);
        head = new node(data, head);
      }

    chain& operator |= (const chain& c)
      {
        ChainAdjoin(*this, c.begin(), c.end());
        return *this;
      }

    iterator        begin()       { return iterator(head); }
    iterator        end  ()       { return iterator(0); }
    const_iterator  begin() const { return const_iterator(head); }
    const_iterator  end  () const { return const_iterator(0); }

  private:
    static bool  EqualContents(const node* n1, const node* n2);

    // friend template <class Iterator>
    // void  ChainAdjoin(chain<Data>& chain, Iterator begin, Iterator end);
};

template <typename Data>
bool  chain<Data>::EqualContents(const chain<Data>::node* n1, const chain<Data>::node* n2)
{
  while (true) {
    if (n1 == 0)
      return n2 == 0;
    if (n2 == 0)
      return false;

    if (n1->data != n2->data)
      return false;

    n1 = n1->next;
    n2 = n2->next;
  }
}

template <class Chain, class Iterator>
void  ChainAdjoin(Chain& chain, Iterator begin, Iterator end)
{
  Chain::node** pput = &chain.__head();
  for (Iterator src = begin; src != end; ++src) {
    while (*pput && (*pput)->data < *src) {
      pput = &(*pput)->next;
    }
    if (*pput == 0 || (*pput)->data != *src) {
      *pput = new Chain::node(*src, *pput);
    }
    pput = &(*pput)->next;
  }
}

#endif
