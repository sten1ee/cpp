#ifndef PG_RE_H
#define PG_RE_H

#include "util/bitset.h"
#include "pg/predefs.h"

#define PURE =0
#define RE_DECL_METHODS \
  virtual void  create(FA* fa, State* beg, State* end) PURE

class RE
{
  public:
    RE_DECL_METHODS;

    virtual ~RE() {}
};

#undef  PURE
#define PURE

class RE_concat : public RE // ab
{
  private:
    RE* a;
    RE* b;

  public:
    RE_DECL_METHODS;

    RE_concat(RE* a, RE* b) : a(a), b(b) {}
   ~RE_concat() { delete a; delete b; }
};

class RE_altern : public RE // a|b
{
  private:
    RE* a;
    RE* b;

  public:
    RE_DECL_METHODS;

    RE_altern(RE* a, RE* b) : a(a), b(b) {}
   ~RE_altern() { delete a; delete b; }
};

class RE_option : public RE // a?
{
  private:
    RE* a;

  public:
    RE_DECL_METHODS;

    RE_option(RE* a) : a(a) {}
   ~RE_option() { delete a; }
};

class RE_iterat : public RE // a*
{
  private:
    RE* a;

  public:
    RE_DECL_METHODS;

    RE_iterat(RE* a) : a(a) {}
   ~RE_iterat() { delete a; }
};

class RE_plusit : public RE // a+
{
  private:
    RE* a;

  public:
    RE_DECL_METHODS;

    RE_plusit(RE* a) : a(a) {}
   ~RE_plusit() { delete a; }
};

class RE_char   : public RE //
{
  private:
    Char  charr;

  public:
    RE_DECL_METHODS;

    RE_char(Char c) : charr(c) {}
};

class RE_charset : public RE//
{
  private:
    BitSet  chars;

  public:
    RE_DECL_METHODS;

    RE_charset(BitSet& _chars) : chars(_chars) {}
};

#endif//PG_RE_H
