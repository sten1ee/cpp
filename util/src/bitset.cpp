#include "util/bitset.h"
#include <string.h>


BitSet::~BitSet()
{
  delete[] word;
}


BitSet::BitSet(member capacity)
{
  wcap = size_t(capacity >> SHIFT);
  if (capacity & MASK) {
    wcap++;
  }
  word = new word_t[wcap];
  memset(word, 0, wcap * sizeof(word_t));
  wdat = 0;
}


BitSet::BitSet(BitSet& set)
{
  word = set.word;
  wcap = set.wcap;
  wdat = set.wdat;
  set.word = 0;
  set.wcap = 0;
  set.wdat = 0;
}

#ifdef OPTION_NO_NONCONST_ASSIGN
BitSet& BitSet::operator  = (BitSet  set)
#else
BitSet& BitSet::operator  = (BitSet& set)
#endif
{
  delete[] word;
  word = set.word;
  wcap = set.wcap;
  wdat = set.wdat;
  set.word = 0;
  set.wcap = 0;
  set.wdat = 0;
  return *this;
}


BitSet  BitSet::clone() const
{
  BitSet  res('*');
  res.word = new word_t[res.wcap = res.wdat = wdat];
  memcpy(res.word, word, wdat * sizeof(word_t));
  return res;
}


void  BitSet::ensureCapacity(size_t mincap)
{
  if (wcap < mincap) {
    size_t  new_wcap = 2 * wcap;
    if (new_wcap < mincap) {
      new_wcap = mincap;
    }
    word_t* new_word = new word_t[new_wcap];

    memcpy(new_word, word, wcap * sizeof(word_t));
    memset(new_word + wcap, 0,
              (new_wcap - wcap) * sizeof(word_t));

    delete[] word;
    word = new_word;
    wcap = new_wcap;
  }
}


void  BitSet::recalcDown_wdat()
{
  while (0 < wdat && word[wdat-1] == 0) {
    wdat--;
  }
}


bool  BitSet::get(member bit) const
{
  size_t n = WORD_IDX(bit);
  if (n < wdat && (word[n] & BIT_MASK(bit))) {
    return true;
  }
  return false;
}


void  BitSet::set(member bit)
{
  size_t n = WORD_IDX(bit);
  if (wdat <= n) {
    ensureCapacity(n + 1);
    wdat = n + 1;
  }
  word[n] |= BIT_MASK(bit);
}


void  BitSet::clear(member bit)
{
  size_t n = WORD_IDX(bit);
  if (n < wdat) {
    word[n] &= ~BIT_MASK(bit);
    if (n == wdat - 1 && word[n] == 0) {
      recalcDown_wdat();
    }
  }
}


BitSet& BitSet::operator |= (const BitSet& set)
{
  if (wdat < set.wdat) {
    ensureCapacity(set.wdat);
    wdat = set.wdat;
  }
  for (size_t n = set.wdat; 0 < n--; ) {
    word[n] |= set.word[n];
  }
  return *this;
}


BitSet& BitSet::operator ^= (const BitSet& set)
{
  if (wdat < set.wdat) {
    ensureCapacity(set.wdat);
    wdat = set.wdat;
  }
  for (size_t n = set.wdat; 0 < n--; ) {
    word[n] ^= set.word[n];
  }

  recalcDown_wdat();

  return *this;
}


BitSet& BitSet::operator &= (const BitSet& set)
{
  if (set.wdat < wdat) {
    memset(word + set.wdat, 0, (wdat - set.wdat) * sizeof(word_t));
    wdat = set.wdat;
  }

  for (size_t n = wdat; 0 < n--; ) {
    word[n] &= set.word[n];
  }

  recalcDown_wdat();

  return *this;
}


BitSet& BitSet::operator -= (const BitSet& set)
{
  size_t  lim;
  bool    recalc;

  if (set.wdat < wdat) {
    lim = set.wdat;
    recalc = false;
  }
  else {
    lim = wdat;
    recalc = true;
  }

  for (size_t n = lim; 0 < n--; ) {
    word[n] &= ~set.word[n];
  }

  if (recalc) {
    recalcDown_wdat();
  }

  return *this;
}


BitSet  BitSet::operator |  (const BitSet& set) const
{
  return clone() |= set;
}


BitSet  BitSet::operator ^  (const BitSet& set) const
{
  return clone() ^= set;
}


BitSet  BitSet::operator &  (const BitSet& set) const
{
  return clone() &= set;
}


BitSet  BitSet::operator -  (const BitSet& set) const
{
  return clone() -= set;
}


bool  BitSet::isSubset(const BitSet& set) const
{
  if (wdat <= set.wdat) {
    for (size_t n = wdat; 0 < n--; ) {
      if (word[n] != (word[n] & set.word[n])) {
        return false;
      }
    }
    return true;
  }
  return false;
}


bool  BitSet::isEqual(const BitSet& set) const
{
  if (wdat == set.wdat) {
    for (size_t n = wdat; 0 < n--; ) {
      if (word[n] != set.word[n]) {
        return false;
      }
    }
    return true;
  }
  return false;
}


BitSet::member  BitSet::countMembers() const
{
  size_t  size = 0;
  for (size_t n = wdat; 0 < n--; ) {
    word_t  w = word[n];
    while (w) {
      w &= w - 1;
      size++;
    }
  }
  return size;
}


BitSet::member  BitSet::nextMember(member bit) const
{
  for (size_t  n = WORD_IDX(++bit); n != wdat; n++) {
    word_t  w = word[n];
    if (0 != w) {    //skip zero words;
      do {           //loop to find non-zero bit:
        if (w & BIT_MASK(bit)) {
          return bit;
        }
      } while (0 != (MASK & ++bit));
    }
    else {
      bit += (1 << SHIFT);
    }
  }
  return NO_SUCH_MEMBER;
}


BitSet::member  BitSet::prevMember(member bit) const
{
  for (size_t  n = WORD_IDX(--bit); n != size_t(-1); n--) {
    word_t  w = word[n];
    if (0 != w) {    //skip zero words:
      do {           //loop to find non-zero bit:
        if (w & BIT_MASK(bit)) {
          return bit;
        }
      } while (MASK != (MASK & --bit));
    }
    else {
      bit -= (1 << SHIFT);
    }
  }
  return NO_SUCH_MEMBER;
}
