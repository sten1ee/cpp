#ifndef calc_sym_h
#define calc_sym_h

#include "lr_symbol.h"


class calc_sym : public lr_symbol
{
  public:

    enum symtype_t
    {
      eof = 2,
      PLUS,
      MINUS,
      MUL,
      DIV,
      REM,
      NOT,
      EQ,
      LESS,
      GRTR,
      QMARK,
      COLON,
      LPAREN,
      RPAREN,
      SEMI,
      NUM
    };

    calc_sym()
      {;}

    calc_sym(symtype_t st)
      : lr_symbol(st)
      {;}

    virtual ~calc_sym() {;}
    /* SYM_INLINE_CODE */
};

template <int ST>
class specz_calc_sym : public calc_sym
{
  public:
    specz_calc_sym()
      : calc_sym((calc_sym::symtype_t)ST)
      {;}
};

template <class VALUE_TYPE>
class value_calc_sym : public calc_sym
{
  public:
    VALUE_TYPE  value;

    value_calc_sym()
      {;}

    value_calc_sym(const VALUE_TYPE& v)
      : value(v)
      {;}

    value_calc_sym(calc_sym::symtype_t st)
      : calc_sym(st)
      {;}

    value_calc_sym(calc_sym::symtype_t st, const VALUE_TYPE& v)
      : calc_sym(st), value(v)
      {;}
};

template <class VALUE_TYPE, int ST>
class specz_value_calc_sym : public value_calc_sym<VALUE_TYPE>
{
  public:
    specz_value_calc_sym()
      : value_calc_sym<VALUE_TYPE>((calc_sym::symtype_t)ST)
      {;}

    specz_value_calc_sym(const VALUE_TYPE& v)
      : value_calc_sym<VALUE_TYPE>((calc_sym::symtype_t)ST, v)
      {;}
};
typedef specz_calc_sym<13>  COLON_sym;
typedef specz_calc_sym<6>  DIV_sym;
typedef specz_calc_sym<9>  EQ_sym;
typedef specz_calc_sym<11>  GRTR_sym;
typedef specz_calc_sym<10>  LESS_sym;
typedef specz_calc_sym<14>  LPAREN_sym;
typedef specz_calc_sym<4>  MINUS_sym;
typedef specz_calc_sym<5>  MUL_sym;
typedef specz_calc_sym<8>  NOT_sym;
typedef specz_value_calc_sym<double, 17>  NUM_sym;
typedef specz_calc_sym<3>  PLUS_sym;
typedef specz_calc_sym<12>  QMARK_sym;
typedef specz_calc_sym<7>  REM_sym;
typedef specz_calc_sym<15>  RPAREN_sym;
typedef specz_calc_sym<16>  SEMI_sym;
typedef specz_calc_sym<2>  eof_sym;


#endif //calc_sym_h
