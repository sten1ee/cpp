#include <ctype.h>
#include <values.h>

#ifdef OPTION_NO_LFN
# include <strstrea.h>
#else
# include <strstream.h>
#endif


#include "util/hugeint.h"
#include "parser/asprint.h"

const char* AbstractSyntaxPrinter::StrId(ET et)
{
  switch (et) {
    case ET_CALL:     return "CallExp";
    case ET_CAST:     return "CastExp";
    case ET_MEMBER:   return "MembExp";
    case ET_CLASSNEW: return "ClassNewExp";
    case ET_ARRNEW:   return "ArrNewExp";
    case ET_SPECNEW:  return "SpecNewExp";
    case ET_INSTOF:   return "instanceof";

    case ET_SPECSYM:  return "<spec sym>";

    case ET_INT:      return "IntLit";
    case ET_LONG:     return "LongLit";
    case ET_FLOAT:    return "FloatLit";
    case ET_DOUBLE:   return "DoubleLit";
    case ET_STRING:   return "StringLit";
    case ET_CHAR:     return "CharLit";

    case ET_PRIMETYPE:return "PrimeType";
    case ET_ARRTYPE:  return "ArrType";
    case ET_CLASSTYPE:return "ClassType";

    case ET_INCE: return "++";
    case ET_DECE: return "--";

    case ET_EINC: return "++";
    case ET_EDEC: return "--";
    case ET_NOT:  return "!";
    case ET_BNOT: return "~";
    case ET_UPLS: return "+";
    case ET_UMNS: return "-";

    case ET_MUL: return "*";
    case ET_DIV: return "/";
    case ET_MOD: return "%";

    case ET_ADD: return "+";
    case ET_SUB: return "-";

    case ET_SHL: return "<<";
    case ET_SHR: return ">>";
    case ET_SHRL:return ">>>";

    case ET_BAND: return "&";
    case ET_BXOR: return "^";
    case ET_BOR:  return "|";

    case ET_LESS:   return "<";
    case ET_LESSEQ: return "<=";
    case ET_GRTR:   return ">";
    case ET_GRTREQ: return ">=";

    case ET_EQ:   return "==";
    case ET_NEQ:  return "!=";

    case ET_AND:  return "&&";
    case ET_OR:   return "||";

    case ET_ASG:    return "=";
    case ET_MULASG: return "*=";
    case ET_DIVASG: return "/=";
    case ET_MODASG: return "%=";
    case ET_ADDASG: return "+=";
    case ET_SUBASG: return "-=";
    case ET_BANDASG:return "&=";
    case ET_BXORASG:return "^=";
    case ET_BORASG: return "|=";
    case ET_SHLASG: return "<<=";
    case ET_SHRASG: return ">>=";
    case ET_SHRLASG:return ">>>=";

    default:  PARSER_ASSERT(!"SUCH CASE"); return "???";
  }
}

enum {
  PRIOMASK    = 0x0F,
  PREFIX      = 0x10,
  POSTFIX     = 0x20,
  INFIX       = PREFIX | POSTFIX,
  LEFT        = 0x40,
  RIGHT       = 0x80,
  MIXED       = LEFT | RIGHT,
};

int  AbstractSyntaxPrinter::Pattern(ET et)
{
  switch (et) {
    case ET_CALL      :
    case ET_SUBSCRIBE :
    case ET_MEMBER    :
    case ET_EINC      :
    case ET_EDEC      : return 1 | POSTFIX;

    case ET_CAST      :
    case ET_NOT       :
    case ET_BNOT      :
    case ET_UPLS      :
    case ET_UMNS      :
    case ET_INCE      :
    case ET_DECE      : return 2 | PREFIX;

    case ET_MUL       :
    case ET_DIV       :
    case ET_MOD       : return 3 | INFIX | LEFT;

    case ET_ADD       :
    case ET_SUB       : return 4 | INFIX | LEFT;

    case ET_SHL       :
    case ET_SHR       :
    case ET_SHRL      : return 5 | INFIX | LEFT;

    case ET_LESS      :
    case ET_LESSEQ    :
    case ET_GRTR      :
    case ET_GRTREQ    : return 6 | INFIX | LEFT;

    case ET_INSTOF    : return 7 | INFIX;

    case ET_EQ        :
    case ET_NEQ       : return 8 | INFIX | LEFT;

    case ET_BAND      : return 9 | INFIX | LEFT;

    case ET_BXOR      : return 10 | INFIX| LEFT;

    case ET_BOR       : return 11 | INFIX| LEFT;

    case ET_AND       : return 12 | INFIX| RIGHT;

    case ET_OR        : return 13 | INFIX| RIGHT;

    case ET_ASG       :
    case ET_MULASG    :
    case ET_DIVASG    :
    case ET_MODASG    :
    case ET_ADDASG    :
    case ET_SUBASG    :
    case ET_BANDASG   :
    case ET_BXORASG   :
    case ET_BORASG    :
    case ET_SHLASG    :
    case ET_SHRASG    :
    case ET_SHRLASG   : return 14 | INFIX| RIGHT;

    case ET_IF        : return 14 | RIGHT;

    case ET_ARRAGG    :
    case ET_CLASSNEW  :
    case ET_ARRNEW    :
    case ET_SPECNEW   :

    case ET_SPECSYM   :
    case ET_SYM       :
    case ET_QSYM      :
    case ET_STUBID    :
    case ET_MEMBSYM   :

    case ET_INT       :
    case ET_LONG      :
    case ET_FLOAT     :
    case ET_DOUBLE    :
    case ET_STRING    :
    case ET_CHAR      :

    case ET_PRIMETYPE :
    case ET_ARRTYPE   :
    case ET_CLASSTYPE : return 0;

    default           : PARSER_ASSERT(!"SUCH CASE"); return 15;
  }
}


bool  AbstractSyntaxPrinter::IsPrefix(ET et)
{
  return et == ET_INCE ||
         et == ET_DECE ||
         et == ET_UPLS ||
         et == ET_UMNS ||
         et == ET_NOT  ||
         et == ET_BNOT;
}


void  AbstractSyntaxPrinter::PrintArgl(Exp::Vector& arg, ostream& os)
{
  os << '(';
  for (size_t i=0; i < arg.size(); i++) {
    if (i) {
      os << ", ";
    }
    PrintExp(arg[i], os);
  }
  os << ')';
}


void  AbstractSyntaxPrinter::PrintChar(wchar_t ch, ostream& os)
{
  switch (ch) {
    case '\n': os << "\\n"; return;
    case '\r': os << "\\r"; return;
    case '\a': os << "\\a"; return;
    case '\b': os << "\\b"; return;
    case '\t': os << "\\t"; return;
    case '\f': os << "\\f"; return;
    case '\0': os << "\\0"; return;
    case '\\': os << "\\\\"; return;
    case '\'': os << "\\\'"; return;
    case '\"': os << "\\\""; return;
  }

  static const char hexdigit[16] =
  {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'
  };
  if (ch <= 0xFF) {
    if (isprint(ch)) {
      os << (char)ch;
      return;
    }
    os << "\\x";
  }
  else {
    os << "\\u"
       << hexdigit[(ch & 0xF000) >> 12]
       << hexdigit[(ch & 0x0F00) >>  8];
  }
  os   << hexdigit[(ch & 0x00F0) >>  4]
       << hexdigit[(ch & 0x000F)];

}


void  AbstractSyntaxPrinter::PrintDouble(double d, ostream& os)
{
  int  sig = 0;
  char buf[128];
  ostrstream ostr(buf, sizeof(buf));
  ostr.precision(21);
  ostr.setf(ios::showpoint);
  ostr << d << ends;
  char* ppoint = strchr(buf, '.');
  if (!ppoint) {
    sig = 0;
  }
  else {
    char* plnull;
    plnull = strchr(ppoint, 'e');
    if (!plnull)
      plnull = strchr(ppoint, 'E');
    if (!plnull)
      plnull = ppoint + strlen(ppoint);
    while (*--plnull == '0')
      ;
    sig = int(plnull - ppoint);
  }
  os.precision(sig);
  os.setf(ios::showpoint);
  os << d;
}


void  AbstractSyntaxPrinter::PrintExp(Exp* exp, ostream& os, int ppat)
{
  int pprio = PRIOMASK & ppat;  //parent pattern
  int cpat  = Pattern(exp->et);
  int cprio = PRIOMASK & cpat;

  bool  bPutParentheses;
  if (pprio && (pprio < cprio || pprio == cprio && ((ppat & MIXED) == MIXED))) {
    bPutParentheses = true;
    os << '(';
  }
  else {
    bPutParentheses = false;
  }

  if (exp->et == ET_SUBSCRIBE) {
    BinExp* bexp = SURE_CAST(exp, BinExp);
    PrintExp(bexp->lop, os, cpat);
    os << '[';
    PrintExp(bexp->rop, os);
    os << ']';
  }
  else if ((exp->et & ET_BINEXP) == ET_BINEXP) {
    BinExp* bexp = SURE_CAST(exp, BinExp);
    PrintExp(bexp->lop, os, cpat | LEFT);
    os << ' ' << StrId(bexp->et) << ' ';
    PrintExp(bexp->rop, os, cpat | RIGHT);
  }
  else if ((exp->et & ET_UNEXP) == ET_UNEXP) {
    UnExp* uexp = SURE_CAST(exp, UnExp);
    if (IsPrefix(uexp->et)) {
      os << StrId(uexp->et);
    }
    PrintExp(uexp->op, os, cpat);
    if (!IsPrefix(uexp->et)) {
      os << StrId(uexp->et);
    }
  }
  else if (exp->et == ET_CALL) {
    CallExp* cexp = SURE_CAST(exp, CallExp);
    PrintExp(cexp->funcDotExp, os, cpat);
    PrintArgl(cexp->argl, os);
  }
  else if ((exp->et & ET_IDEXP) == ET_IDEXP) {
    if (exp->et == ET_QSYM) {
      QSymExp* qsexp = SURE_CAST(exp, QSymExp);
      PrintExp(qsexp->scopeIdExp, os);
      os << '.';
    }
    if (exp->et == ET_MEMBSYM) {
      MembSymExp* qsexp = SURE_CAST(exp, MembSymExp);
      PrintExp(qsexp->objExp, os, cpat);
      os << '.';
    }
    DotExp* seexp = SURE_CAST(exp, DotExp);
    if (seexp->sym != TheStubSymbol)
      os << seexp->sym;
    else
      os << "<stub-sym>";
  }
  else if (exp->et == ET_QSYM) {
    QSymExp* qsexp = SURE_CAST(exp, QSymExp);
    PrintExp(qsexp->scopeIdExp, os);
    os << '.';
    if (qsexp->sym != TheStubSymbol)
      os << qsexp->sym;
    else
      os << "<stub-sym>";
  }
  else if (exp->et == ET_STUBID) {
    PARSER_ASSERT(SURE_CAST(exp, StubIdExp));
    os << "<stub-id>";
  }
  else if (exp->et == ET_INT) {
    IntExp* intExp = SURE_CAST(exp, IntExp);
    if (intExp->val > INT32_MAX) {
      os << "0x";
      hex(os);
      os << intExp->val;
    }
    else {
      os << intExp->val;
    }
  }
  else if (exp->et == ET_LONG) {
    LongExp* longExp = SURE_CAST(exp, LongExp);
    if (longExp->val.high != 0 ||
        longExp->val.low > INT32_MAX) {
      os << "0x";
      hex(os);
      char fillch = os.fill('0');
      os.width(8);
      os << longExp->val.high;
      os.width(8);
      os << longExp->val.low;
      os.fill(fillch);
      dec(os);
    }
    else {
      os << longExp->val.low;
    }
    os << "L";
  }
  else if (exp->et == ET_FLOAT) {
    FloatExp* fexp = SURE_CAST(exp, FloatExp);
    PrintDouble(fexp->val, os);
    os << "F";
  }
  else if (exp->et == ET_DOUBLE) {
    DoubleExp* dexp = SURE_CAST(exp, DoubleExp);
    PrintDouble(dexp->val, os);
  }
  else if (exp->et == ET_CHAR) {
    CharExp* cexp = SURE_CAST(exp, CharExp);
    os << '\'';
    PrintChar(cexp->val, os);
    os << '\'';
  }
  else if (exp->et == ET_STRING) {
    StringExp* sexp = SURE_CAST(exp, StringExp);
    os << '"';
    for (size_t i = 0; i < sexp->val.length(); i++) {
      PrintChar(sexp->val[i], os);
    }
    os << '"';
  }
  else if (exp->et == ET_CAST) {
    CastExp* cexp = SURE_CAST(exp, CastExp);
    os << '(';
    PrintExp(cexp->targetTypeExp, os);
    os << ')';
    PrintExp(cexp->castedExp, os, cpat);
  }
  else if (exp->et == ET_PRIMETYPE) {
    os << SURE_CAST(exp, PrimeTypeExp)->toStr();
  }
  else if (exp->et == ET_CLASSTYPE) {
    ClassTypeExp* clsexp = SURE_CAST(exp, ClassTypeExp);
    PrintExp(clsexp->classIdExp, os);
  }
  else if (exp->et == ET_ARRTYPE) {
    ArrTypeExp* arrexp = SURE_CAST(exp, ArrTypeExp);
    PrintExp(arrexp->typeExp, os);
    for (int i=arrexp->ndims; 0 < i--; ) {
      os << "[]";
    }
  }
  else if (exp->et == ET_ARRNEW) {
    ArrNewExp* anewExp = SURE_CAST(exp, ArrNewExp);
    os << "new ";
    PrintExp(anewExp->typeExp, os);
    for (int i=0; i < anewExp->dim.size(); i++) {
      os << '[';
      if (anewExp->dim[i]) {
        PrintExp(anewExp->dim[i], os);
      }
      os << "]";
    }
  }
  else if (exp->et == ET_CLASSNEW) {
    ClassNewExp* cnewExp = SURE_CAST(exp, ClassNewExp);
    os << "new ";
    PrintExp(cnewExp->classTypeExp, os);
    PrintArgl(cnewExp->argl, os);
  }
  else if (exp->et == ET_SPECNEW) {
    SpecNewExp* snewExp = SURE_CAST(exp, SpecNewExp);
    os << "new (";
    PrintExp(snewExp->exp, os);
    os << ')';
  }
  else if (exp->et == ET_SPECSYM) {
    switch (SURE_CAST(exp, SpecSymExp)->ss) {
      case SS_THIS  : os << "this";   break;
      case SS_SUPER : os << "super";  break;
      case SS_TRUE  : os << "true";   break;
      case SS_FALSE : os << "false";  break;
      case SS_NULL  : os << "null";   break;
      default: PARSER_ASSERT(!"case");
    }
  }
  else if (exp->et == ET_NULL) {
    os << "null";
  }
  else if (exp->et == ET_INSTOF) {
    InstofExp* instofExp = SURE_CAST(exp, InstofExp);
    PrintExp(instofExp->checkedExp, os, cpat);
    os << " instanceof ";
    PrintExp(instofExp->objTypeExp, os, cpat);
  }
  else if (exp->et == ET_IF) {
    IfExp* ifExp = SURE_CAST(exp, IfExp);
    PrintExp(ifExp->condExp, os, cpat | LEFT);
    os << " ? ";
    PrintExp(ifExp->thenExp, os, cpat);
    os << " : ";
    PrintExp(ifExp->elseExp, os, cpat | RIGHT);
  }
  else if (exp->et == ET_ARRAGG) {
    ArrAggExp* arrAggExp = SURE_CAST(exp, ArrAggExp);
    os << "{ ";
    for (int i=0; i < arrAggExp->val.size(); i++) {
      if (i) {
        os << ", ";
      }
      PrintExp(arrAggExp->val[i], os);
    }
    os << " }";
  }
  else {
    PARSER_ASSERT(!"SUCH CASE");
  }

  if (bPutParentheses) {
    os << ')';
  }
}


void  AbstractSyntaxPrinter::PrintExp(Exp* exp, ostream& os)
{
  PrintExp(exp, os, 0);
}
