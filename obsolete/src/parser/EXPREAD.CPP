//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// parser/expread.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include  "parser/syn.h"


AbstractSyntax::PT  SYN::Syn::TT2PT(TT tt)
{
  switch (tt) {
    case TT_void    : return PT_VOID;
    case TT_byte    : return PT_BYTE;
    case TT_short   : return PT_SHORT;
    case TT_int     : return PT_INT;
    case TT_long    : return PT_LONG;
    case TT_float   : return PT_FLOAT;
    case TT_double  : return PT_DOUBLE;
    case TT_boolean : return PT_BOOLEAN;
    case TT_char    : return PT_CHAR;
  }
  PARSER_ASSERT(!"CASE");
  return PT(0);
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_Prefixes()
{
  ET et;
  switch (tt) {
    case TT_LogNot   : et = ET_NOT;  break;
    case TT_BitNot   : et = ET_BNOT; break;
    case TT_Plus     : et = ET_UPLS; break;
    case TT_Minus    : et = ET_UMNS; break;
    case TT_Increment: et = ET_INCE; break;
    case TT_Decrement: et = ET_DECE; break;
    default          : return ReadLevel_Postfixes();
  }
  UnExp* uexp = new UnExp(lc, et);
  nextToken();
  uexp->op = ReadLevel_Prefixes();
  return uexp;
}


AbstractSyntax::Exp* SYN::Syn::ReadNewExp()
{
  PARSER_ASSERT(tt == TT_new);
  LinCol _lc = lc;
  nextToken();
  if (tt == TT_OpenBrek) {
    nextToken();
    SpecNewExp* snewExp = new SpecNewExp(_lc, ReadExp());
    skipToken(TT_ClosBrek, SErr_MissingClosBrek);
    return snewExp;
  }
  TypeExp* typeExp;
  if (IsPrimeType(tt)) {
    typeExp = new PrimeTypeExp(lc, TT2PT(tt));
    nextToken();
  }
  else if (tt == TT_Symbol) {
    IdExp* idExp = ReadIdExp();
    ClassTypeExp* classTypeExp = new ClassTypeExp(lc, idExp);
    if (tt == TT_OpenKvadBrek) {
      typeExp = classTypeExp;
    }
    else {
      Exp::Vector argl;
      if (tt != TT_OpenBrek) {
        error(SErr_ArglMissingOpenBrek);
      }
      else {
        argl = ReadArgList();
      }
      return new ClassNewExp(_lc, classTypeExp, argl);
    }
  }
  else {
    error(SErr_NewMissingClassId);
    return new StubIdExp(lc);
  }
  PARSER_ASSERT(tt == TT_OpenKvadBrek);
  ArrNewExp* anewExp = new ArrNewExp(_lc, typeExp);
  bool bDimMissed = false;
  while (tt == TT_OpenKvadBrek) {
    nextToken();
    if (tt == TT_ClosKvadBrek) {
      bDimMissed = true;
      anewExp->dim.push(0);
    }
    else {
      if (bDimMissed) {
        error(SErr_ArrNewInvalidDim);
      }
      anewExp->dim.push(ReadExp());
    }
    skipToken(TT_ClosKvadBrek, SErr_MissingClosKvadBrek);
  }
  return anewExp;
}


AbstractSyntax::IdExp*  SYN::Syn::ReadIdExp(error_t err)
{
  if (tt != TT_Symbol) {
    PARSER_ASSERT(err != ErrNone);
    error(err);
    return new StubIdExp(lc);
  }
  IdExp* idExp = new SymExp(lc, lex->valueSymbol());
  nextToken();
  while (tt == TT_Dot) {
    nextToken();
    if (tt != TT_Symbol) {
      error(SErr_IdentExpectedOnLeftOfDot);
      break;
    }
    idExp = new QSymExp(lc, idExp, lex->valueSymbol());
    nextToken();
  }
  return idExp;
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_Term()
{
  //read term:
  Exp* exp;
  switch (tt) {
    case TT_Int     : exp = new IntExp(lc, lex->valueInt());           break;
    case TT_Long    : exp = new LongExp(lc, lex->valueLong());         break;
    case TT_Float   : exp = new FloatExp(lc, lex->valueFloat());       break;
    case TT_Double  : exp = new DoubleExp(lc, lex->valueDouble());     break;
    case TT_String  : exp = new StringExp(lc, lex->valueString());     break;
    case TT_Char    : exp = new CharExp(lc, lex->valueChar());         break;
    case TT_this    : exp = new SpecSymExp(lc, SS_THIS);               break;
    case TT_super   : exp = new SpecSymExp(lc, SS_SUPER);              break;
    case TT_null    : exp = new SpecSymExp(lc, SS_NULL);               break;
    case TT_true    : exp = new SpecSymExp(lc, SS_TRUE);               break;
    case TT_false   : exp = new SpecSymExp(lc, SS_FALSE);              break;
    case TT_Symbol  : return ReadIdExp();
    case TT_new     : return ReadNewExp();
    case TT_OpenBrek: {
      nextToken();
      bool tryCast = (tt != TT_OpenBrek);
      Exp* exp = ReadExp();
      LinCol  closLC = lc;
      skipToken(TT_ClosBrek, SErr_MissingClosBrek);

      TypeExp* typeExp = SAFE_CAST(exp, TypeExp);
      if (typeExp) {
        return  new CastExp(closLC, typeExp, ReadLevel_Prefixes()); //!!!
      }
      IdExp* idexp = SAFE_CAST(exp, IdExp);
      if (tryCast && idexp && (tt == TT_OpenBrek || tt == TT_Symbol)) {
        return new CastExp(closLC,
                           new ClassTypeExp(closLC, idexp),
                           ReadLevel_Postfixes()); //!!!
      }
      return exp;
    }

    case TT_EOF:
      error(SErr_ExpExpectedInsteadEOF);
      return new StubIdExp(lc);

    default:
      if (IsPrimeType(tt)) {
        exp = new PrimeTypeExp(lc, TT2PT(tt));
        break;
      }
      error(SErr_TermExpected);
      return new StubIdExp(lc);
  }
  nextToken();
  return exp;
}


AbstractSyntax::Exp* SYN::Syn::ReadLevel_Postfixes()
{
  Exp* exp = ReadLevel_Term();
  while (true) {
    if (tt == TT_Increment) {
      exp = new UnExp(lc, ET_EINC, exp);
      nextToken();
    }
    else if (tt == TT_Decrement) {
      exp = new UnExp(lc, ET_EDEC, exp);
      nextToken();
    }
    else if (tt == TT_Dot) {
      LinCol  dotLC = lc;
      nextToken();
      if (tt == TT_Symbol) {
        exp = new MembSymExp(lc, exp, lex->valueSymbol());
        nextToken();
      }
      else {
        error(SErr_IdentExpectedOnRightOfDot);
        //exp = new MemberExp(lc, idexp, TheStubSymbol);
      }
    }
    else if (tt == TT_OpenKvadBrek) {
      LinCol  openLC = lc;
      nextToken();
      if (tt == TT_ClosKvadBrek) {
        TypeExp* typeExp = SAFE_CAST(exp, TypeExp);
        if (!typeExp) {
          IdExp* idExp = SAFE_CAST(exp, IdExp);
          if (!idExp) {
            error(SErr_TypeExpectedOnLeft);
            idExp = new StubIdExp(lc);
          }
          typeExp = new ClassTypeExp(lc, idExp);
        }
        ArrTypeExp* arrTypeExp = SAFE_CAST(typeExp, ArrTypeExp);
        if (!arrTypeExp) {
          arrTypeExp = new ArrTypeExp(openLC, typeExp, 0);
        }
        arrTypeExp->ndims++;
        exp = arrTypeExp;
        nextToken();
      }
      else {
        exp = new BinExp(openLC, ET_SUBSCRIBE, exp, ReadExp());
        skipToken(TT_ClosKvadBrek, SErr_MissingClosKvadBrek);
      }
    }
    else if (tt == TT_OpenBrek) {
      DotExp* funcDotExp = SAFE_CAST(exp, DotExp);
      if (!funcDotExp) {
        error(SErr_FuncIdExpected);
        delete exp;
        funcDotExp = new StubIdExp(lc);
      }
      CallExp* callExp = new CallExp(lc, funcDotExp);
      exp = callExp;
      callExp->argl = ReadArgList();
    }
    else {
      return exp;
    }
  }
}


AbstractSyntax::Exp::Vector  SYN::Syn::ReadArgList()
{
  skipToken(TT_OpenBrek, SErr_ArglMissingOpenBrek);
  Exp::Vector argl;
  if (tt == TT_ClosBrek) { //f()
    nextToken();
    return argl;
  }
  while (true) {
    if (!CanContainExp(tt)) {
      error(SErr_ArglMissingClosBrek);
      break;
    }
    argl.push(ReadExp());
    if (tt == TT_ClosBrek) {
      nextToken();
      break;
    }
    if (tt == TT_Comma) {
      nextToken();
    }
    else if (CanContainExp(tt)) {
      error(SErr_ArglMissingComma);
    }
  }
  return argl;
}


AbstractSyntax::TypeExp*  SYN::Syn::ReadTypeExp(error_t err)
{
  TypeExp* typeExp;
  if (tt == TT_Symbol) {
    typeExp = ReadObjTypeExp();
  }
  else if (IsPrimeType(tt)) {
    typeExp = new PrimeTypeExp(lc, TT2PT(tt));
    nextToken();
  }
  else {
    error(err);
    typeExp = new StubTypeExp(lc);
  }

  if (tt == TT_OpenKvadBrek) {
    typeExp = new ArrTypeExp(typeExp->lc, typeExp, ReadDims());
  }
  return typeExp;
}


int   SYN::Syn::ReadDims()
{
  int ndims = 0;
  if (tt == TT_OpenKvadBrek) {
    do {
      nextToken();
      ndims++;
    } while (skipToken(TT_ClosKvadBrek, SErr_MissingClosKvadBrek) &&
             tt == TT_OpenKvadBrek);
  }
  return ndims;
}


AbstractSyntax::ObjTypeExp*  SYN::Syn::ReadObjTypeExp(error_t err)
{
  ClassTypeExp* classTypeExp = ReadClassTypeExp(err);
  if (tt == TT_OpenKvadBrek) {
    return new ArrTypeExp(classTypeExp->lc, classTypeExp, ReadDims());
  }
  return classTypeExp;
}


AbstractSyntax::ClassTypeExp*  SYN::Syn::ReadClassTypeExp(error_t err)
{
  if (tt != TT_Symbol) {
    error(err);
    return new ClassTypeExp(lc, new StubIdExp(lc));
  }
  IdExp* idExp = ReadIdExp();
  return new ClassTypeExp(idExp->lc, idExp);
}

