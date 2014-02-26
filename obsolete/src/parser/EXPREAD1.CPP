//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// parser/expread.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include  "parser/syn.h"


AbstractSyntax::Exp* SYN::Syn::ReadExp()
{
  return ReadLevel_Assigns();
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_Assigns()
{
  Exp* exp = ReadLevel_LogOrs();
  ET   et;
  switch (tt) {
    case TT_Question        : {
      IfExp* ifExp = new IfExp(lc, exp);
      nextToken();
      ifExp->thenExp = ReadExp();
      skipToken(TT_DoubleDot, SErr_QMMissingDD);
      ifExp->elseExp = ReadExp();
      return ifExp;
    }
    case TT_Assign          : et = ET_ASG;     break;
    case TT_MulAssign       : et = ET_MULASG;  break;
    case TT_DivAssign       : et = ET_DIVASG;  break;
    case TT_ModAssign       : et = ET_MODASG;  break;
    case TT_PlusAssign      : et = ET_ADDASG;  break;
    case TT_MinusAssign     : et = ET_SUBASG;  break;
    case TT_BitAndAssign    : et = ET_BANDASG; break;
    case TT_BitXorAssign    : et = ET_BXORASG; break;
    case TT_BitOrAssign     : et = ET_BORASG;  break;
    case TT_LeftShiftAssign : et = ET_SHLASG;  break;
    case TT_RightShiftAssign: et = ET_SHRASG;  break;
    case TT_RightLogShAssign: et = ET_SHRLASG; break;
    default                 : return exp;
  }
  BinExp* bexp = new BinExp(lc, et, exp);
  nextToken();
  bexp->rop = ReadLevel_Assigns();
  return bexp;
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_LogOrs()
{
  Exp* exp = ReadLevel_LogAnds();
  if (tt == TT_LogOr) {
    BinExp* bexp = new BinExp(lc, ET_OR, exp);
    nextToken();
    bexp->rop = ReadLevel_LogOrs();
    exp = bexp;
  }
  return exp;
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_LogAnds()
{
  Exp* exp = ReadLevel_BitOrs();
  if (tt == TT_LogAnd) {
    BinExp* bexp = new BinExp(lc, ET_AND, exp);
    nextToken();
    bexp->rop  = ReadLevel_LogAnds();
    exp = bexp;
  }
  return exp;
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_BitOrs()
{
  Exp* exp = ReadLevel_BitXors();
  while (tt == TT_BitOr) {
    BinExp* bexp = new BinExp(lc, ET_BOR, exp);
    nextToken();
    bexp->rop = ReadLevel_BitXors();
    exp = bexp;
  }
  return exp;
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_BitXors()
{
  Exp* exp = ReadLevel_BitAnds();
  while (tt == TT_BitXor) {
    BinExp* bexp = new BinExp(lc, ET_BXOR, exp);
    nextToken();
    bexp->rop = ReadLevel_BitAnds();
    exp = bexp;
  }
  return exp;
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_BitAnds()
{
  Exp* exp = ReadLevel_Eqs();
  while (tt == TT_BitAnd) {
    BinExp* bexp = new BinExp(lc, ET_BAND, exp);
    nextToken();
    bexp->rop = ReadLevel_Eqs();
    exp = bexp;
  }
  return exp;
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_Eqs()
{
  Exp* exp = ReadLevel_InstanceOf();
  ET   et;
  while (true) {
    switch (tt) {
      case TT_Eq    : et = ET_EQ;  break;
      case TT_NotEq : et = ET_NEQ; break;
      default       : return exp;
    }
    BinExp* bexp = new BinExp(lc, et, exp);
    nextToken();
    bexp->rop = ReadLevel_InstanceOf();
    exp = bexp;
  }
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_InstanceOf()
{
  Exp* exp = ReadLevel_Comps();
  if (tt == TT_instanceof) {
    InstofExp* instofExp = new InstofExp(lc, exp);
    nextToken();
    instofExp->objTypeExp = ReadObjTypeExp();
    exp = instofExp;
  }
  return exp;
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_Comps()
{
  Exp* exp = ReadLevel_Shifts();
  ET   et;
  while (true) {
    switch (tt) {
      case TT_Less     : et = ET_LESS;   break;
      case TT_LessEq   : et = ET_LESSEQ; break;
      case TT_Greater  : et = ET_GRTR;   break;
      case TT_GreaterEq: et = ET_GRTREQ; break;
      default          : return exp;
    }
    BinExp* bexp = new BinExp(lc, et, exp);
    nextToken();
    bexp->rop = ReadLevel_Shifts();
    exp = bexp;
  }
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_Shifts()
{
  Exp* exp = ReadLevel_Adds();
  ET   et;
  while (true) {
    switch (tt) {
      case TT_LeftShift    : et = ET_SHL;  break;
      case TT_RightShift   : et = ET_SHR;  break;
      case TT_RightLogShift: et = ET_SHRL; break;
      default              : return exp;
    }
    BinExp* bexp = new BinExp(lc, et, exp);
    nextToken();
    bexp->rop = ReadLevel_Adds();
    exp = bexp;
  }
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_Adds()
{
  Exp* exp = ReadLevel_Muls();
  ET   et;
  while (true) {
    switch (tt) {
      case TT_Plus : et = ET_ADD; break;
      case TT_Minus: et = ET_SUB; break;
      default      : return exp;
    }
    BinExp* bexp = new BinExp(lc, et, exp);
    nextToken();
    bexp->rop = ReadLevel_Muls();
    exp = bexp;
  }
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_Muls()
{
  Exp* exp = ReadLevel_Prefixes();
  ET   et;
  while (true) {
    switch (tt) {
      case TT_Mul : et = ET_MUL; break;
      case TT_Div : et = ET_DIV; break;
      case TT_Mod : et = ET_MOD; break;
      default     : return exp;
    }
    BinExp* bexp = new BinExp(lc, et, exp);
    nextToken();
    bexp->rop = ReadLevel_Prefixes();
    exp = bexp;
  }
}


AbstractSyntax::Exp*  SYN::Syn::ReadArrAggExp(int ndims)
{
  PARSER_ASSERT(tt == TT_OpenFigBrek);
  ArrAggExp* arrAggExp = new ArrAggExp(lc);
  nextToken();
  ndims--;

  while (true) {
    Exp* nextVal;
    if (ndims) {
      nextVal = ReadArrAggExp(ndims);
    }
    else {
      nextVal = ReadExp();
    }
    arrAggExp->val.push(nextVal);

    bool  bCommaSkiped = false;
    if (tt == TT_Comma) {
      nextToken();
      bCommaSkiped = true;
    }
    if (tt == TT_ClosFigBrek) {
      nextToken();
      return arrAggExp;
    }
    if (tt == TT_EOF) {
      error(SErr_ArrAggMisingClosFigBrek);
      return arrAggExp;
    }

    if (!bCommaSkiped) { //force to get comma
      skipToken(TT_Comma, Err_MissingComma);
    }
  }
}
