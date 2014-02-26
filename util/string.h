#ifndef UTIL_STRING_H
#define UTIL_STRING_H

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <iostream>

#ifndef   UTIL_DEBUG_H
#include "util/debug.h"
#endif
#ifndef   UTIL_TYPES_H
#include "util/types.h"
#endif
#ifndef   UTIL_MINMAX_H
#include "util/minmax.h"
#endif

#ifndef _MSC_VER
# pragma intrisinc memcpy
# pragma intrisinc memmove
# pragma intrisinc memset
# pragma intrisinc strlen
#endif

/*
template <class CHAR>
int strlen(const CHAR* str)
{
  const CHAR* ptr = str;
  while (*ptr != CHAR(0))
    ptr++;
  return ptr - str;
}
*/

template <class ITERATOR>
void  destroy_objects(ITERATOR begin, ITERATOR end)
{
  for (ITERATOR i = begin; i != end; ++i)
    delete *i;
}

template <class ITERATOR>
void  destroy_arrays(ITERATOR begin, ITERATOR end)
{
  for (ITERATOR i = begin; i != end; ++i)
    delete[] *i;
}


template <class CHAR> class String
{
  public:
    typedef CHAR   Char;

  protected:
    struct Rep
    {
      size_t  len;
      size_t  maxlen;
      size_t  share;
      Char    chr[1];
    };

    Char*   rep_chr;

    static  Rep     TheEmptyRep;
    static  Char*   TheEmptyRep_chr;
    static  size_t  ResizeIncrement;

    static  Rep*  rep_chr2rep(void* chr)
      {
        return (Rep*)((char*)chr - offsetof(Rep, chr));
      }
    static  const Rep*  rep_chr2rep(const void* chr)
      {
        return (Rep*)((char*)chr - offsetof(Rep, chr));
      }

          Rep*  rep()       { return rep_chr2rep(rep_chr); }
    const Rep*  rep() const { return rep_chr2rep(rep_chr); }

    static Char* new_rep(size_t maxlen);
    static void  release(Rep* rep)
      {
        if (rep->share)
          rep->share--;
        else
          delete[] (char*)rep;
      }

    Char* copyRep(size_t maxlen);
    Char* ownRep(size_t minlen, size_t resizeInc)
      {
        if (rep()->share || rep()->maxlen < minlen)
          return copyRep(minlen + resizeInc);
        return rep_chr;
      }
    void  possessRep(Char* _rep_chr)
      {
        if (rep_chr != _rep_chr) {
          release(rep());
          rep_chr = _rep_chr;
        }
      }
    void  possessOwnRep(size_t minlen, size_t resizeInc)
      {
        possessRep(ownRep(minlen, resizeInc));
      }
    void  possessOwnRep()
      {
        possessOwnRep(rep()->len, 0);
      }

  public:
   ~String();

    String();
    String(const String& s);
    String(const Char* s);
    String(const Char* s, size_t len);

    String& operator = (const String& s);
    String& operator = (const Char* s);

    const Char* c_str()  const { return rep_chr; }
    size_t      length() const { return rep()->len; }
    bool        isNull() const { return length() == 0; }

    Char  operator [](size_t pos) const
      {
        UTIL_ASSERT(pos <= rep()->len);
        return rep_chr[pos];
      }

    Char  getAt(size_t pos) const
      {
        UTIL_ASSERT(pos <= rep()->len);
        return rep_chr[pos];
      }

    void  putAt(size_t pos, Char c);

    void  trimToLength();
    
    static  hash_t  Hash(const Char* s, size_t n);
            hash_t  hash() const { return Hash(rep_chr, length()); }

//------------------------------------------------------------------------------
    void  ensureFreeLength(size_t freelen, size_t resinc=ResizeIncrement)
      { possessOwnRep(rep()->len + freelen, resinc); }

//------------------------------------------------------------------------------
    String& append(Char c);
    String& append(const Char* s, size_t n);
    String& append(const Char* s)
      { return append(s, strlen(s)); }
    String& append(const String& s)
      { return append(s.c_str(), s.length()); }

//------------------------------------------------------------------------------
    String& insertAt(size_t pos, Char c);
    String& insertAt(size_t pos, const Char* s, size_t n);
    String& insertAt(size_t pos, const Char* s)
      { return insertAt(pos, s, strlen(s)); }
    String& insertAt(size_t pos, const String& s)
      { return insertAt(pos, s.c_str(), s.length()); }

//------------------------------------------------------------------------------
    String& removeAt(size_t pos, size_t n);

//------------------------------------------------------------------------------
    int   compareTo(const String& s) const;

    bool  operator == (const String& s) const;
    bool  operator != (const String& s) const
      { return !operator == (s); }
    bool  operator < (const String& s) const
      { return compareTo(s) < 0; }
    bool  operator <=(const String& s) const
      { return compareTo(s) <= 0; }

//------------------------------------------------------------------------------
    typedef const Char* const_iterator;
    typedef       Char*       iterator;

    const_iterator  begin() const { return rep_chr; }
          iterator  begin()       { return rep_chr; }

    const_iterator  end  () const { return rep_chr + length(); }
          iterator  end  ()       { return rep_chr + length(); }
};

template <class CHAR>
typename String<CHAR>::Rep
String<CHAR>::TheEmptyRep = { 0, 0, 1, {CHAR(0)}};
template <class CHAR>
typename String<CHAR>::Char*
String<CHAR>::TheEmptyRep_chr = String<CHAR>::TheEmptyRep.chr;
template <class CHAR>
size_t  String<CHAR>::ResizeIncrement = 32;

template <class CHAR>
CHAR* String<CHAR>::new_rep(size_t maxlen)
  {
    size_t nbytes = sizeof(Rep) + maxlen*sizeof(Char);
    Rep* rep = (Rep*)new char[nbytes];
    memset(rep, 0, nbytes);
    rep->maxlen = maxlen;
    return rep->chr;
  }

template <class CHAR>
CHAR* String<CHAR>::copyRep(size_t maxlen)
  {
    UTIL_ASSERT(rep()->len <= maxlen);
    Char*  copy_rep_chr = new_rep(maxlen);
    memcpy(copy_rep_chr, rep_chr, rep()->len*sizeof(Char));
    rep_chr2rep(copy_rep_chr)->len  = rep()->len;
    return copy_rep_chr;
  }

template <class CHAR>
String<CHAR>::~String()
  {
    release(rep());
  }

template <class CHAR>
String<CHAR>::String()
  {
    rep_chr = TheEmptyRep_chr;
    rep()->share++;
  }

template <class CHAR>
String<CHAR>::String(const String& s)
  {
    rep_chr = const_cast<String&>(s).rep_chr;
    rep()->share++;
  }

template <class CHAR>
String<CHAR>::String(const Char* s)
  {
    if (*s == 0) {
      rep_chr = TheEmptyRep_chr;
      rep()->share++;
    }
    else {
      size_t len = strlen(s);
      rep_chr = new_rep(len);
      memcpy(rep_chr, s, len*sizeof(Char));
      rep()->len = len;
    }
  }

template <class CHAR>
String<CHAR>::String(const Char* s, size_t len)
  {
    rep_chr = new_rep(len);
    memcpy(rep_chr, s, len*sizeof(Char));
    rep()->len = len;
  }

template <class CHAR>
String<CHAR>& String<CHAR>::operator = (const String& s)
  {
    Rep* old_rep = rep();
    rep_chr = const_cast<String&>(s).rep_chr;
    rep()->share++;
    release(old_rep);
    return *this;
  }

template <class CHAR>
String<CHAR>& String<CHAR>::operator = (const Char* s)
  {
    Rep* old_rep = rep();
    if (*s == 0) {
      rep_chr = TheEmptyRep_chr;
      rep()->share++;
    }
    else {
      size_t len = strlen(s);
      rep_chr = new_rep(len);
      memcpy(rep_chr, s, len*sizeof(Char));
      rep()->len = len;
    }
    release(old_rep);
    return *this;
  }

template <class CHAR>
void  String<CHAR>::putAt(size_t pos, CHAR c)
  {
    UTIL_ASSERT(pos < rep()->len);
    possessOwnRep();
    rep_chr[pos] = c;
  }

template <class CHAR>
void  String<CHAR>::trimToLength()
  {
    if (rep()->len < rep()->maxlen) {
      Char* trimed_rep_chr = copyRep(rep()->len);
      release(rep());
      rep_chr = trimed_rep_chr;
    }
  }

//------------------------------------------------------------------------------
template <class CHAR>
String<CHAR>& String<CHAR>::append(const Char* s, size_t n)
  {
    Char* chr = ownRep(rep()->len + n, ResizeIncrement);
    memcpy(chr + rep()->len, s, n*sizeof(Char));
    possessRep(chr);
    rep()->len += n;
    return *this;
  }

template <class CHAR>
String<CHAR>& String<CHAR>::append(Char c)
  {
    possessOwnRep(rep()->len + 1, ResizeIncrement);
    rep_chr[rep()->len++] = c;
    return *this;
  }
//------------------------------------------------------------------------------
template <class CHAR>
String<CHAR>& String<CHAR>::insertAt(size_t pos, const Char* s, size_t n)
  {
    Char* chr = ownRep(rep()->len + n, ResizeIncrement);
    memmove(chr + pos + n, chr + pos, (rep()->len - pos)*sizeof(Char));
    memcpy(chr + pos, s, n*sizeof(Char));
    possessRep(chr);
    rep()->len += n;
    return *this;
  }

template <class CHAR>
String<CHAR>& String<CHAR>::insertAt(size_t pos, Char c)
  {
    possessOwnRep(rep()->len + 1, ResizeIncrement);
    memmove(rep_chr + pos + 1, rep_chr + pos, (rep()->len - pos)*sizeof(Char));
    rep_chr[pos] = c;
    rep()->len++;
    return *this;
  }

template <class CHAR>
String<CHAR>& String<CHAR>::removeAt(size_t pos, size_t n)
  {
    UTIL_ASSERT(pos + n <= rep()->len);
    possessOwnRep();
    memmove(rep_chr + pos, rep_chr + pos + n, (rep()->len - (pos + n))*sizeof(Char));
    rep()->len -= n;
    memset(rep_chr + rep()->len, 0, (rep()->maxlen - rep()->len)*sizeof(Char));
    return *this;
  }

//------------------------------------------------------------------------------
template <class CHAR>
bool  String<CHAR>::operator == (const String& s) const
  {
    if (rep()->len != s.rep()->len)
      return false;
    return 0 == memcmp(rep_chr, s.rep_chr, rep()->len*sizeof(Char));
  }

template <class CHAR>
int   String<CHAR>::compareTo(const String& s) const
  {
    size_t minlen = min(rep()->len, s.rep()->len);
    int res = memcmp(rep_chr, s.rep_chr, minlen*sizeof(Char));
    if (res != 0)
      return res;
    if (rep()->len < s.rep()->len)
      return INT_MIN;
    if (rep()->len > s.rep()->len)
      return INT_MAX;
    return 0;
  }

template <class CHAR>
hash_t  String<CHAR>::Hash(const Char* s, size_t n)
  {
    hash_t  h = 0;

    if (n < 16) {
      while (0 < n--) {
        h = (h * 37) + (hash_t)s[n];
      }
    }
    else {
      // only sample some characters
      size_t  skip = n / 8;
      while (skip <= n) {
        n -= skip;
        h = (h * 39) + (hash_t)s[n];
      }
    }
    return h;
  }


typedef String<char> string;

inline
std::ostream& operator << (std::ostream& os, const string& s)
{
  return os << s.c_str();
}

template <class CHAR>
void  destroy_objects(String<CHAR*>& str)
{
  destroy_objects(str.begin(), str.end());
}

template <class CHAR>
void  destroy_arrays(String<CHAR*>& str)
{
  destroy_arrays(str.begin(), str.end());
}


#endif //UTIL_STRING_H
