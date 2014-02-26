#include "util/lexer.h"
#include "util/debug.h"
#include <ctype.h>
#include <math.h>

//----------------------------------------------------------------------
#ifndef LEXER_SOURCE_DEFINED

LexerSource::LexerSource(istream& is)
  : is(is),
    lineno(0),
    line(128, DOUBLE_INCREMENT)
{
}


const char* LexerSource::nextLine()
{
  if (is.peek() == EOF) {
    return 0;
  }
  ///////////////////////
  //if (is.peek() == '$') {
  //  is.get();
  //  return 0;
  //}
  //cout << lineno << ": " << line.c_str() << endl;
  ///////////////////////
  line.flush();
  lineno++;
  while (true) {
    int c = is.get();
    switch (c) {
      case '\n':
      case EOF :
        line.push('\0');
        return &line[0];
      default:
        line.push((char)c);
    }
  }
}


LinCol  LexerSource::pos(const char* cp)
{
  UTIL_ASSERT(&line[0] <= cp && cp <= &line[0] + line.size() - 1);
  unsigned short l = (unsigned short)lineno;
  unsigned short c = (unsigned short)((cp - &line[0]) + 1);
  return LinCol(l, c);
}

#endif//LEXER_SOURCE_DEFINED
//----------------------------------------------------------------------
#ifndef LEXER_ERRHAN_DEFINED
const char* LexerErrHan::getErrorMessage(error_t)
{
  return "";
}

void  LexerErrHan::error(const LinCol& lc, error_t err)
{
  os << "$ERROR " << lc << getErrorMessage(err) << endl;
  nerrors++;
  UTIL_ASSERT(nerrors <= 7);
}
#endif//LEXER_ERRHAN_DEFINED
//----------------------------------------------------------------------
#ifndef LEXER_SYMTAB_DEFINED
LexerSymbol TheStubSymbolStruct = { TT_NOTOKEN, { 0 } };

symbol_t TheStubSymbol = TheStubSymbolStruct.strid;

symbol_t  LexerSymTab::getSymbol(const char* s, size_t l)
{
  LexerSymbolHashTraits::Key key = { s, l };
  LexerSymbol* sym = table.get(key);
  if (!sym) {
    sym = (LexerSymbol*)new char[sizeof(LexerSymbol) + l];
    sym->keyword = TT_NOTOKEN;
    strncpy(sym->strid, s, l);
    sym->strid[l] = 0;
    table.put(sym);
  }
  return sym->strid;
}
#endif//LEXER_SYMTAB_DEFINED
//----------------------------------------------------------------------

inline
int char2digit(char ch)
{
  UTIL_ASSERT(isdigit(ch));
  return ch - '0';
}


inline
int char2xdigit(char ch)
{
  UTIL_ASSERT(isxdigit(ch));
  return isdigit(ch) ? ch - '0' :
         isupper(ch) ? ch - 'A' + 10 :
                       ch - 'a' + 10;
}


bool  Lexer::nextLine()
{
  const char* nl = source->nextLine();
  if (nl) {
    cp = nl;
    return true;
  }
  return false;
}


bool  Lexer::skipCComment()
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


void  Lexer::error(error_t err)
{
  errorHandler->error(source->pos(cp), err);
}


toktag_t  Lexer::readNumber()
{
  int_t  iv = 0;

  if (*cp == '0') { //search for hex and oct formats:
    cp++;
    if (*cp == 'x') { //0x
      cp++;
      if (!isxdigit(*cp)) {
        error(LErr_BadHexNumber);
      }
      else {
        do {
          iv *= 16;
          iv += char2xdigit(*cp++);
        } while (isxdigit(*cp));
      }
    }
    else if (isdigit(*cp)) { //0<d>
      if (!(*cp < '8')) {
        error(LErr_BadOctNumber);
      }
      else {
        do {
          iv *= 8;
          iv += char2digit(*cp++);
        } while (isdigit(*cp) && *cp < '8');
      }
    }
  }
  else {
    while (isdigit(*cp)) {
      iv *= 10;
      iv += char2digit(*cp++);
    }
  }

  if (*cp != '.') {
    value_int = iv;
    return  (toktag = TT_INT);
  }

  cp++; //to skip the '.'
  float_t  fv = iv;
  float_t  exp = 1.0;
  while (isdigit(*cp)) {
    fv += char2digit(*cp++) * (exp /= 10);
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
      fv *= pow(10.0, negexp ? -exp : exp);
    }
  }
  value_float = fv;
  return (toktag = TT_FLOAT);
}


bool  Lexer::scanChar(char_t& oc)
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
/***********************************************
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
***********************************************/
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


toktag_t  Lexer::readChar()
{
  UTIL_ASSERT(cp[-1] == '\'');
  if (scanChar(value_char) && *cp == '\'') {
    cp++;
  }
  else {
    error(LErr_UnterminatedChar);
  }

  return (toktag = TT_CHAR);
}


toktag_t  Lexer::readString()
{
  UTIL_ASSERT(cp[-1] == '\"');
  char_t  ch;
  value_string = "";
  while (*cp != '"') {
    if (scanChar(ch)) {
      value_string.append(ch);
    }
    else {
      error(LErr_UnterminatedString);
      return (toktag = TT_STRING);
    }
  }
  cp++;
  return (toktag = TT_STRING);
}


toktag_t  Lexer::readSymbol()
{
  UTIL_ASSERT(isalpha(*cp) || *cp == '_');
  const char* s = cp;
  do {
    cp++;
  } while (isalnum(*cp) || *cp == '_');
  symbol_t sym = symbolTable->getSymbol(s, size_t(cp - s));
  if (isKeyword(sym)) {
    return toKeyword(sym);
  }
  value_symbol = sym;
  return (toktag = TT_SYMBOL);
}


toktag_t  Lexer::nextToken()
{
  toktag = TT_NOTOKEN;
AGAIN:
  while (isspace(*cp)) {
    cp++;
  }
  sp = cp;
  switch (*cp) {
    case '\"':
      cp++;
      return readString();
    case '\'':
      cp++;
      return readChar();
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
      }
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
