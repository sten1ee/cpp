#ifndef UTIL_BITSET_H
#define UTIL_BITSET_H

#include "util/types.h"


class BitSet
{
  public:
    typedef long    member;
  protected:
    typedef uint32  word_t;

    enum {
      SHIFT            =  5,               //type is int
      MASK             = (1 << SHIFT) - 1, //type is word_t
      DEFAULT_CAPACITY = 64                //type is member
    };

    static  size_t  WORD_IDX(member bit)
    {
      return size_t(bit >> SHIFT);
    }

    static  word_t  BIT_MASK(member bit)
    {
      return word_t(1) << int(bit & MASK);
    }

  protected:
    word_t* word; //array of words representing the BitSet
    size_t  wcap; //the allocated capacity of 'word'
    size_t  wdat; //if (wdat > 0) then word[wdat-1] is the last non null word

            BitSet(char) {} //'do nothing' constructor for internal use

    void    ensureCapacity(size_t mincap); //provides at least mincap words
    void    recalcDown_wdat();

  public:
           ~BitSet();
            BitSet(member capacity=DEFAULT_CAPACITY);
            BitSet(BitSet& set); //`set' is ANNIHILATED !
                                 //use BitSet(set.clone()) to prevent it !
#ifdef OPTION_NO_NONCONST_ASSIGN
    BitSet& operator  = (BitSet  set);
#else
    BitSet& operator  = (BitSet& set); //`set' is ANNIHILATED !
#endif                                 //use ... = set.clone() to prevent it !

    BitSet  clone() const; //clone `this' object without harming it in any way.

    bool    get  (member bit) const;
    void    set  (member bit);
    void    clear(member bit);

    bool    isSubset(const BitSet& set) const;
    bool    isEqual (const BitSet& set) const;
    bool    isEmpty () const { return wdat == 0; }

    BitSet& operator |= (const BitSet& set);
    BitSet& operator ^= (const BitSet& set);
    BitSet& operator &= (const BitSet& set);
    BitSet& operator -= (const BitSet& set);

    BitSet  operator |  (const BitSet& set) const;
    BitSet  operator ^  (const BitSet& set) const;
    BitSet  operator &  (const BitSet& set) const;
    BitSet  operator -  (const BitSet& set) const;

    bool    operator == (const BitSet& set) const { return  isEqual(set); }
    bool    operator != (const BitSet& set) const { return !isEqual(set); }

    enum  { NO_SUCH_MEMBER = -1 }; //type is member

    member  firstMember() const { return nextMember(member(-1)); }
    member  lastMember()  const { return prevMember(member(wdat) << SHIFT); }
    member  nextMember(member bit) const;
    member  prevMember(member bit) const;
    member  countMembers()         const;

    word_t  _get_word(size_t n) const { return word[n]; }
    size_t  _get_wdat()         const { return wdat;    }
    size_t  _get_wcap()         const { return wcap;    }
};

#endif //UTIL_BITSET_H
