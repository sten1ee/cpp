#include "parser/lex.h"
#include "parser/debug.h"
#include <ctype.h>
#include <math.h>


inline
int char2digit(char ch)
{
  PARSER_ASSERT(isdigit(ch));
  return ch - '0';
}


inline
int char2xdigit(char ch)
{
  PARSER_ASSERT(isxdigit(ch));
  return isdigit(ch) ? ch - '0' :
         isupper(ch) ? ch - 'A' + 10 :
                       ch - 'a' + 10;
}


bool  Lex::nextLine()
{
  const char* nl = source->nextLine();
  if (nl) {
    cp = nl;
    return true;
  }
  return false;
}


bool  Lex::skipCComment()
{
  int opened = 1;
  while (opened) {
    switch (*cp++) {
      case '*' :
        if (*cp == '/') {
          cp++;
          opened--;
        }
        break;

      case '/' :
        if (*cp == '*') {
          cp++;
          opened++;
        }
        else if (*cp == '/') {
      case 0: if (!nextLine()) {
                error(LErr_CommentNotClosed);
                return false;
              }
        }
        break;
    }
  }
  return true;
}


void  Lex::error(error_t err)
{
  errorHandler->error(source->pos(cp), err);
}


TT  Lex::readNumber()
{
  HugeInt     hi = 0;

  if (*cp == '0') { //search for hex and oct formats:
    cp++;
    if (*cp == 'x') { //0x
      cp++;
      if (!isxdigit(*cp)) {
        error(LErr_BadHexNumber);
      }
      else {
        do {
          hi *= 16;
          hi += char2xdigit(*cp++);
        } while (isxdigit(*cp));
      }
      goto RET_INT;    }
    else if (isdigit(*cp)) { //0<d>
      if (!(*cp < '8')) {
        error(LErr_BadOctNumber);
      }
      else {
        do {
          hi *= 8;
          hi += char2digit(*cp++);
        } while (isdigit(*cp) && *cp < '8');
      }
      goto RET_INT;
    }
  }

  while (isdigit(*cp)) {
    hi *= 10;
    hi += char2digit(*cp++);
  }

  long double res;
  res = hi.to_long_double();
  if (*cp == 'd' || *cp == 'D') {
    goto RET_DOUBLE;
  }
  if (*cp == 'f' || *cp == 'F') {
    goto RET_FLOAT;
  }
  if (*cp == '.') {
    cp++;
    goto PARSE_FRACT;
  }

RET_INT:
  if (*cp == 'l' || *cp == 'L') {
    *cp++;
    if (HugeInt::UINT64_MAX < hi) {
      error(LErr_LongConstOutOfRange);
      value_long.high = UINT32_MAX;
      value_long.low  = UINT32_MAX;
    }
    else {
      value_long.high = hi.uint64_high();
      value_long.low  = hi.uint64_low();
    }
    PARSER_DEBUGIN( tokenType = TT_Long; )
    return TT_Long;
  }
  else {
    if (HugeInt(UINT32_MAX) < hi) {
      error(LErr_IntConstOutOfRange);
      value_int = UINT32_MAX;
    }
    else {
      value_int = hi.uint64_low();
    }
    PARSER_DEBUGIN( tokenType = TT_Int; )
    return TT_Int;
  }

PARSE_FRACT:
  long double exp;
  exp = 1.0;
  while (isdigit(*cp)) {
    res += char2digit(*cp++) * (exp /= 10);
  }
  if (*cp == 'e' || *cp == 'E') { // we have the 123.45e-10 format:
    cp++;
    bool negexp = false;
    switch (*cp) {
      case '-': negexp = true;
      case '+': cp++;
    }
    if (!isdigit(*cp)) {
      error(LErr_BadFloatNumber);
    }
    else {
      exp = 0;
      do {
        exp *= 10;
        exp += char2digit(*cp++);
      } while (isdigit(*cp));
      #ifdef OPTION_NO_LD_LIBROUTINES
      {
        long double  d = 1.0;
        while (exp--) {
          d *= 10;
        }
        res = (negexp ? res/d : res*d);
      }
      #else
        res *= powl(10, negexp ? -exp : exp);
      #endif
    }
  }
  if (*cp == 'f' || *cp == 'F') {
  RET_FLOAT:
    cp++;
    value_float = (float)res;
    //if (value_float != res) {
    //  error(LErr_FloatConstOutOfPrecision);
    //}
    PARSER_DEBUGIN( tokenType = TT_Float; )
    return TT_Float;
  }

  if (*cp == 'd' || *cp == 'D') {
  RET_DOUBLE:
    cp++;
  }
  value_double = (double)res;
  //if (value_double != res) {
  //  error(LErr_DoubleConstOutOfPrecision);
  //}
  PARSER_DEBUGIN( tokenType = TT_Double; )
  return TT_Double;
}


bool  Lex::scanChar(wchar_t& oc)
{
  if (*cp == '\\') {
    switch (*++cp) {
      case '\'':  oc = '\''; break; // single quote
      case '\"':  oc = '\"'; break; // double quote
      case '\\':  oc = '\\'; break; // back slash
      case '0' :  oc = '\0'; break; // string terminator
      case 'a' :  oc = '\a'; break; // bell
      case 'b' :  oc = '\b'; break; // backspace
      case 'f' :  oc = '\f'; break; // form feed
      case 'n' :  oc = '\n'; break; // linefeed
      case 'r' :  oc = '\r'; break; // carriage return
      case 't' :  oc = '\t'; break; // htab
      case 'x' :  {
                    wchar_t res = 0;
                    for (int i = 2; 0 < i--; ) {
                      if (!isxdigit(*++cp)) {
                        error(LErr_BadHexChar);
                        return true;
                      }
                      res <<= 4;
                      res |= char2xdigit(*cp);
                    }
                    oc = res;
                    break;
                  }
      case 'u' :  {
                    wchar_t res = 0;
                    for (int i = 4; 0 < i--; ) {
                      if (!isxdigit(*++cp)) {
                        error(LErr_BadUniChar);
                        return true;
                      }
                      res <<= 4;
                      res |= char2xdigit(*cp);
                    }
                    oc = res;
                    break;
                  }
      default  :  error(LErr_BadEscapeChar);
                  return true;
      case  0  :  return false; //fatal error: caller should know!


    }
  }
  else {
    if (*cp == 0) {
      return false; //fatal error: caller should know!
    }
    oc = *cp;
  }
  cp++;
  return true;
}


TT  Lex::readChar()
{
  PARSER_ASSERT(cp[-1] == '\'');
  if (scanChar(value_char) && *cp == '\'') {
    cp++;
  }
  else {
    error(LErr_UnterminatedChar);
  }
  PARSER_DEBUGIN( tokenType = TT_Char; )
  return TT_Char;
}


TT  Lex::readString()
{
  PARSER_ASSERT(cp[-1] == '\"');
  wchar_t  ch;
  value_string = L"";
  while (*cp != '"') {
    if (scanChar(ch)) {
      value_string.append(ch);
    }
    else {
      error(LErr_UnterminatedString);
      PARSER_DEBUGIN( tokenType = TT_String; )
      return TT_String;
    }
  }
  cp++;
  PARSER_DEBUGIN( tokenType = TT_String; )
  return TT_String;
}


TT  Lex::readSymbol()
{
  PARSER_ASSERT(isalpha(*cp) || *cp == '_');
  const char* s = cp;
  do {
    cp++;
  } while (isalnum(*cp) || *cp == '_');
  symbol_t sym = symbolTable->getSymbol(s, size_t(cp - s));
  if (isKeyword(sym)) {
    return toKeyword(sym);
  }
  value_symbol = sym;
  PARSER_DEBUGIN( tokenType = TT_Symbol; )
  return TT_Symbol;
}


TT  Lex::nextToken()
{
  PARSER_DEBUGIN( tokenType = TT_NOTOKEN; )
AGAIN:
  while (isspace(*cp)) {
    cp++;
  }
  sp = cp;
  switch (*cp) {
    case '!':
      cp++;
      switch (*cp) {
        case '=':
          cp++;
          return TT_NotEq;
      }
      return TT_LogNot;
    case '\"':
      cp++;
      return readString();
    case '%':
      cp++;
      switch (*cp) {
        case '=':
          cp++;
          return TT_ModAssign;
      }
      return TT_Mod;
    case '&':
      cp++;
      switch (*cp) {
        case '&':
          cp++;
          return TT_LogAnd;
        case '=':
          cp++;
          return TT_BitAndAssign;
      }
      return TT_BitAnd;
    case '\'':
      cp++;
      return readChar();
    case '(':
      cp++;
      return TT_OpenBrek;
    case ')':
      cp++;
      return TT_ClosBrek;
    case '*':
      cp++;
      switch (*cp) {
        case '=':
          cp++;
          return TT_MulAssign;
      }
      return TT_Mul;
    case '+':
      cp++;
      switch (*cp) {
        case '+':
          cp++;
          return TT_Increment;
        case '=':
          cp++;
          return TT_PlusAssign;
      }
      return TT_Plus;
    case ',':
      cp++;
      return TT_Comma;
    case '-':
      cp++;
      switch (*cp) {
        case '-':
          cp++;
          return TT_Decrement;
        case '=':
          cp++;
          return TT_MinusAssign;
      }
      return TT_Minus;
    case '.':
      if (!isdigit(cp[1])) { // .1 (float number)
        cp++;
        return TT_Dot;
      }
    case '/':
      cp++;
      switch (*cp) {
        case '*':
          cp++;
          skipCComment();
          goto AGAIN;
        case '/':
          while (*++cp)
            ;
          nextLine();
          goto AGAIN;
        case '=':
          cp++;
          return TT_DivAssign;
      }
      return TT_Div;
    case ':':
      cp++;
      return TT_DoubleDot;
    case '?':
      cp++;
      return TT_Question;
    case ';':
      cp++;
      return TT_Semicol;
    case '<':
      cp++;
      switch (*cp) {
        case '<':
          cp++;
          switch (*cp) {
            case '=':
              cp++;
              return TT_LeftShiftAssign;
          }
          return TT_LeftShift;
        case '=':
          cp++;
          return TT_LessEq;
      }
      return TT_Less;
    case '=':
      cp++;
      switch (*cp) {
        case '=':
          cp++;
          return TT_Eq;
      }
      return TT_Assign;
    case '>':
      cp++;
      switch (*cp) {
        case '=':
          cp++;
          return TT_GreaterEq;
        case '>':
          cp++;
          switch (*cp) {
            case '=':
              cp++;
              return TT_RightShiftAssign;
            case '>':
              cp++;
              switch (*cp) {
                case '=':
                  cp++;
                  return TT_RightLogShAssign;
              }
              return TT_RightLogShift;
          }
          return TT_RightShift;
      }
      return TT_Greater;
    case '[':
      cp++;
      return TT_OpenKvadBrek;
    case ']':
      cp++;
      return TT_ClosKvadBrek;
    case '^':
      cp++;
      switch (*cp) {
        case '=':
          cp++;
          return TT_BitXorAssign;
      }
      return TT_BitXor;
    case '{':
      cp++;
      return TT_OpenFigBrek;
    case '|':
      cp++;
      switch (*cp) {
        case '=':
          cp++;
          return TT_BitOrAssign;
        case '|':
          cp++;
          return TT_LogOr;
      }
      return TT_BitOr;
    case '}':
      cp++;
      return TT_ClosFigBrek;
    case '~':
      cp++;
      return TT_BitNot;
    case 0:
      if (nextLine()) {
        goto AGAIN;
      }
      else {
        return TT_EOF;
      }
    default:
      if (isalpha(*cp) || *cp == '_') {
        return readSymbol();
      }
      if (isdigit(*cp) || *cp == '.') {
        return readNumber();
      }
      error(LErr_IllegalCharacter);
      cp++;
      goto AGAIN;
  }
}
