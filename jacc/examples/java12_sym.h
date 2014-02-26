#ifndef java12_sym_h
#define java12_sym_h

#include "lr_symbol.h"


class java12_sym : public lr_symbol
{
  public:

    enum symtype_t
    {
      eof = 2,
      BOOLEAN,
      BYTE,
      SHORT,
      INT,
      LONG,
      CHAR,
      FLOAT,
      DOUBLE,
      LBRACK,
      RBRACK,
      IDENTIFIER,
      DOT,
      SEMICOLON,
      MULT,
      COMMA,
      LBRACE,
      RBRACE,
      EQ,
      LPAREN,
      RPAREN,
      COLON,
      PACKAGE,
      IMPORT,
      PUBLIC,
      PROTECTED,
      PRIVATE,
      STATIC,
      ABSTRACT,
      FINAL,
      NATIVE,
      SYNCHRONIZED,
      TRANSIENT,
      VOLATILE,
      CLASS,
      EXTENDS,
      IMPLEMENTS,
      VOID,
      THROWS,
      THIS,
      SUPER,
      INTERFACE,
      IF,
      ELSE,
      SWITCH,
      CASE,
      DEFAULT,
      DO,
      WHILE,
      FOR,
      BREAK,
      CONTINUE,
      RETURN,
      THROW,
      TRY,
      CATCH,
      FINALLY,
      NEW,
      PLUSPLUS,
      MINUSMINUS,
      PLUS,
      MINUS,
      COMP,
      NOT,
      DIV,
      MOD,
      LSHIFT,
      RSHIFT,
      URSHIFT,
      LT,
      GT,
      LTEQ,
      GTEQ,
      INSTANCEOF,
      EQEQ,
      NOTEQ,
      AND,
      XOR,
      OR,
      ANDAND,
      OROR,
      QUESTION,
      MULTEQ,
      DIVEQ,
      MODEQ,
      PLUSEQ,
      MINUSEQ,
      LSHIFTEQ,
      RSHIFTEQ,
      URSHIFTEQ,
      ANDEQ,
      XOREQ,
      OREQ,
      INTEGER_LITERAL,
      FLOATING_POINT_LITERAL,
      BOOLEAN_LITERAL,
      CHARACTER_LITERAL,
      STRING_LITERAL,
      NULL_LITERAL,
      STRICTFP,
      CONST,
      GOTO
    };

    java12_sym()
      {;}

    java12_sym(symtype_t st)
      : lr_symbol(st)
      {;}

    virtual ~java12_sym() {;}
    /* SYM_INLINE_CODE */
};

template <int ST>
class specz_java12_sym : public java12_sym
{
  public:
    specz_java12_sym()
      : java12_sym((java12_sym::symtype_t)ST)
      {;}
};

template <class VALUE_TYPE>
class value_java12_sym : public java12_sym
{
  public:
    VALUE_TYPE  value;

    value_java12_sym()
      {;}

    value_java12_sym(const VALUE_TYPE& v)
      : value(v)
      {;}

    value_java12_sym(java12_sym::symtype_t st)
      : java12_sym(st)
      {;}

    value_java12_sym(java12_sym::symtype_t st, const VALUE_TYPE& v)
      : java12_sym(st), value(v)
      {;}
};

template <class VALUE_TYPE, int ST>
class specz_value_java12_sym : public value_java12_sym<VALUE_TYPE>
{
  public:
    specz_value_java12_sym()
      : value_java12_sym<VALUE_TYPE>((java12_sym::symtype_t)ST)
      {;}

    specz_value_java12_sym(const VALUE_TYPE& v)
      : value_java12_sym<VALUE_TYPE>((java12_sym::symtype_t)ST, v)
      {;}
};
typedef specz_java12_sym<30>  ABSTRACT_sym;
typedef specz_java12_sym<78>  AND_sym;
typedef specz_java12_sym<81>  ANDAND_sym;
typedef specz_java12_sym<92>  ANDEQ_sym;
typedef specz_java12_sym<3>  BOOLEAN_sym;
typedef specz_value_java12_sym<bool, 97>  BOOLEAN_LITERAL_sym;
typedef specz_java12_sym<52>  BREAK_sym;
typedef specz_java12_sym<4>  BYTE_sym;
typedef specz_java12_sym<47>  CASE_sym;
typedef specz_java12_sym<57>  CATCH_sym;
typedef specz_java12_sym<8>  CHAR_sym;
typedef specz_value_java12_sym<char, 98>  CHARACTER_LITERAL_sym;
typedef specz_java12_sym<36>  CLASS_sym;
typedef specz_java12_sym<23>  COLON_sym;
typedef specz_java12_sym<17>  COMMA_sym;
typedef specz_java12_sym<64>  COMP_sym;
typedef specz_java12_sym<102>  CONST_sym;
typedef specz_java12_sym<53>  CONTINUE_sym;
typedef specz_java12_sym<48>  DEFAULT_sym;
typedef specz_java12_sym<66>  DIV_sym;
typedef specz_java12_sym<85>  DIVEQ_sym;
typedef specz_java12_sym<49>  DO_sym;
typedef specz_java12_sym<14>  DOT_sym;
typedef specz_java12_sym<10>  DOUBLE_sym;
typedef specz_java12_sym<45>  ELSE_sym;
typedef specz_java12_sym<20>  EQ_sym;
typedef specz_java12_sym<76>  EQEQ_sym;
typedef specz_java12_sym<37>  EXTENDS_sym;
typedef specz_java12_sym<31>  FINAL_sym;
typedef specz_java12_sym<58>  FINALLY_sym;
typedef specz_java12_sym<9>  FLOAT_sym;
typedef specz_value_java12_sym<double, 96>  FLOATING_POINT_LITERAL_sym;
typedef specz_java12_sym<51>  FOR_sym;
typedef specz_java12_sym<103>  GOTO_sym;
typedef specz_java12_sym<72>  GT_sym;
typedef specz_java12_sym<74>  GTEQ_sym;
typedef specz_java12_sym<13>  IDENTIFIER_sym;
typedef specz_java12_sym<44>  IF_sym;
typedef specz_java12_sym<38>  IMPLEMENTS_sym;
typedef specz_java12_sym<25>  IMPORT_sym;
typedef specz_java12_sym<75>  INSTANCEOF_sym;
typedef specz_java12_sym<6>  INT_sym;
typedef specz_value_java12_sym<int, 95>  INTEGER_LITERAL_sym;
typedef specz_java12_sym<43>  INTERFACE_sym;
typedef specz_java12_sym<18>  LBRACE_sym;
typedef specz_java12_sym<11>  LBRACK_sym;
typedef specz_java12_sym<7>  LONG_sym;
typedef specz_java12_sym<21>  LPAREN_sym;
typedef specz_java12_sym<68>  LSHIFT_sym;
typedef specz_java12_sym<89>  LSHIFTEQ_sym;
typedef specz_java12_sym<71>  LT_sym;
typedef specz_java12_sym<73>  LTEQ_sym;
typedef specz_java12_sym<63>  MINUS_sym;
typedef specz_java12_sym<88>  MINUSEQ_sym;
typedef specz_java12_sym<61>  MINUSMINUS_sym;
typedef specz_java12_sym<67>  MOD_sym;
typedef specz_java12_sym<86>  MODEQ_sym;
typedef specz_java12_sym<16>  MULT_sym;
typedef specz_java12_sym<84>  MULTEQ_sym;
typedef specz_java12_sym<32>  NATIVE_sym;
typedef specz_java12_sym<59>  NEW_sym;
typedef specz_java12_sym<65>  NOT_sym;
typedef specz_java12_sym<77>  NOTEQ_sym;
typedef specz_java12_sym<100>  NULL_LITERAL_sym;
typedef specz_java12_sym<80>  OR_sym;
typedef specz_java12_sym<94>  OREQ_sym;
typedef specz_java12_sym<82>  OROR_sym;
typedef specz_java12_sym<24>  PACKAGE_sym;
typedef specz_java12_sym<62>  PLUS_sym;
typedef specz_java12_sym<87>  PLUSEQ_sym;
typedef specz_java12_sym<60>  PLUSPLUS_sym;
typedef specz_java12_sym<28>  PRIVATE_sym;
typedef specz_java12_sym<27>  PROTECTED_sym;
typedef specz_java12_sym<26>  PUBLIC_sym;
typedef specz_java12_sym<83>  QUESTION_sym;
typedef specz_java12_sym<19>  RBRACE_sym;
typedef specz_java12_sym<12>  RBRACK_sym;
typedef specz_java12_sym<54>  RETURN_sym;
typedef specz_java12_sym<22>  RPAREN_sym;
typedef specz_java12_sym<69>  RSHIFT_sym;
typedef specz_java12_sym<90>  RSHIFTEQ_sym;
typedef specz_java12_sym<15>  SEMICOLON_sym;
typedef specz_java12_sym<5>  SHORT_sym;
typedef specz_java12_sym<29>  STATIC_sym;
typedef specz_java12_sym<101>  STRICTFP_sym;
typedef specz_value_java12_sym<char*, 99>  STRING_LITERAL_sym;
typedef specz_java12_sym<42>  SUPER_sym;
typedef specz_java12_sym<46>  SWITCH_sym;
typedef specz_java12_sym<33>  SYNCHRONIZED_sym;
typedef specz_java12_sym<41>  THIS_sym;
typedef specz_java12_sym<55>  THROW_sym;
typedef specz_java12_sym<40>  THROWS_sym;
typedef specz_java12_sym<34>  TRANSIENT_sym;
typedef specz_java12_sym<56>  TRY_sym;
typedef specz_java12_sym<70>  URSHIFT_sym;
typedef specz_java12_sym<91>  URSHIFTEQ_sym;
typedef specz_java12_sym<39>  VOID_sym;
typedef specz_java12_sym<35>  VOLATILE_sym;
typedef specz_java12_sym<50>  WHILE_sym;
typedef specz_java12_sym<79>  XOR_sym;
typedef specz_java12_sym<93>  XOREQ_sym;
typedef specz_java12_sym<2>  eof_sym;


#endif //java12_sym_h
