#ifndef ab_sym_h
#define ab_sym_h

#include "lr_symbol.h"


class ab_sym : public lr_symbol
{
  public:

    enum symtype_t
    {
      eof = 2,
      a,
      b
    };

    ab_sym()
      {;}

    ab_sym(symtype_t st)
      : lr_symbol(st)
      {;}

    virtual ~ab_sym() {;}
    /* SYM_INLINE_CODE */
};

template <int ST>
class specz_ab_sym : public ab_sym
{
  public:
    specz_ab_sym()
      : ab_sym((ab_sym::symtype_t)ST)
      {;}
};

template <class VALUE_TYPE>
class value_ab_sym : public ab_sym
{
  public:
    VALUE_TYPE  value;

    value_ab_sym()
      {;}

    value_ab_sym(const VALUE_TYPE& v)
      : value(v)
      {;}

    value_ab_sym(ab_sym::symtype_t st)
      : ab_sym(st)
      {;}

    value_ab_sym(ab_sym::symtype_t st, const VALUE_TYPE& v)
      : ab_sym(st), value(v)
      {;}
};

template <class VALUE_TYPE, int ST>
class specz_value_ab_sym : public value_ab_sym<VALUE_TYPE>
{
  public:
    specz_value_ab_sym()
      : value_ab_sym<VALUE_TYPE>((ab_sym::symtype_t)ST)
      {;}

    specz_value_ab_sym(const VALUE_TYPE& v)
      : value_ab_sym<VALUE_TYPE>((ab_sym::symtype_t)ST, v)
      {;}
};
typedef specz_ab_sym<3>  a_sym;
typedef specz_ab_sym<4>  b_sym;
typedef specz_ab_sym<2>  eof_sym;
typedef value_ab_sym< int >  int_sym;


#endif //ab_sym_h
