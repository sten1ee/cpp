#include "util/types.h"
#include "pg/re.h"
#include "pg/reread.h"
#include <iostream.h>
#include <string.h>
#include <ctype.h>

#define CHAR_FIRST 0
#define CHAR_LAST 127

void  ErrorReadRE(const char* errmsg)
{
  cerr << "!!!\n!!! Error ReadRE: " << errmsg << "\n!!!\n";
}

/*
RE -> RE | RE
RE -> RE   RE
RE -> RE *
RE -> RE +
RE -> RE ?
RE -> RE_char
//RE -> RE_charset
*/

RE* ReadRE_charset(istream& is)
{
  is.get(); //skip the '['
  Char c = is.get();
  BitSet  charset;
  bool negate = false;
  if (c == '^') {
    negate = true;
    c = is.get();
  }
  while (c != ']' && c != EOF) {
    if (is.peek() == '-') {
      is.get();
      Char d = is.get();
      if (c >= d) {
        ErrorReadRE("CharSet: bad range !");
        Char t = d; d = c; c = t;
      }
      for (Char i = c; i <= d; i++) {
        charset.set((BitSet::member)i);
      }
    }
    else {
      charset.set((BitSet::member)c);
    }
    c = is.get();
  }
  if (c != ']') {
    ErrorReadRE("CharSet: missing ']' !");
  }

  if (negate) {
    BitSet  negCharset;
    for (Char c = CHAR_FIRST; c <= CHAR_LAST; c++) {
      if (!charset.get((BitSet::member)c)) {
        negCharset.set((BitSet::member)c);
      }
    }
    return  new RE_charset(negCharset);
  }
  else {
    return  new RE_charset(charset);
  }
}


RE* ReadRE_term(istream& is)
{
  char c = (char)is.peek();
  if (c == '(') {
    is.get();
    RE* re = ReadRE(is);
    if (is.peek() != ')')
      ErrorReadRE("Expected ')'");
    else
      is.get();
    return re;
  }

  if (c == '[') {
    return ReadRE_charset(is);
  }
  if (strchr("|*+?)", c)) {
    return new RE_char(EPS_CHAR);
  }
  else {
    return new RE_char(is.get());
  }
}


RE* ReadRE_iterats(istream& is)
{
  RE* re = ReadRE_term(is);
  while (true) {
    switch (is.peek()) {
      case '*':
        is.get();
        re = new RE_iterat(re);
        break;
      case '+':
        is.get();
        re = new RE_plusit(re);
        break;
      case '?':
        is.get();
        re = new RE_option(re);
        break;
      default:
        return re;
    }
  }
}


RE* ReadRE_concats(istream& is)
{
  RE* re = ReadRE_iterats(is);
  while (!strchr("|) \n\t", is.peek())) {
    re = new RE_concat(re, ReadRE_iterats(is));
  }
  return re;
}


RE* ReadRE_alterns(istream& is)
{
  RE* re = ReadRE_concats(is);
  while ('|' == is.peek()) {
    is.get();
    re = new RE_altern(re, ReadRE_concats(is));
  }
  return re;
}


RE* ReadRE(istream& is)
{
  return ReadRE_alterns(is);
}
