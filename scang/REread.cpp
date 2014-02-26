#include "reread.h"
#include "util/types.h"
#include "util/assert.h"
#include "util/sortvec.h"
#include <strstream.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/*
RE -> RE | RE
RE -> RE   RE
RE -> RE *
RE -> RE +
RE -> RE ?
RE -> RE_char
RE -> RE_charset
*/
REContext::REContext()
{
  DefineMacro("upper", "[A-Z]");
  DefineMacro("lower", "[a-z]");
  DefineMacro("digit", "[0-9]");
  DefineMacro("alpha", "{upper}|{lower}");
}


void  REContext::ErrorReadRE(const char* errmsg)
{
  cerr << "!!!\n!!! Error ReadRE: " << errmsg << "\n!!!\n";
}


void  REContext::DefineMacro(const char* name, const char* body)
{
  macros.insert(new Macro(name, ReadRE(istrstream(const_cast<char*>(body)))));
}


RE* REContext::GetMacro(const char* name)
{
  Macro::set::finger f = macros.find(name);
  return f ? (*f)->body->clone() : 0;
}


RE* REContext::Read_macro(istream& is)
{
  ASSERT(is.peek() == '{');
  is.get(); //skip the '{'

  char  name[80];
  char* put = name;
  char* zpos= name + (sizeof(name) - 1);
  while (isalnum(is.peek()) || '_' == is.peek()) {
    *put++ = (char)is.get();
    if (put == zpos) {
      ErrorReadRE("Macro name too long !");
      break;
    }
  }
  *put = 0;
  if (put == name) {
    ErrorReadRE("Zero-length macro name !");
  }
  if (is.peek() != '}') {
     ErrorReadRE("Macro reference missing '}' !");
  }
  is.get(); // skip the '}'
  RE* m = GetMacro(name);
  if (!m) {
    char buf[100];
    sprintf(buf, "Undefined macro `%s' !", name);
    ErrorReadRE(buf);
    return new RE_char('?');
  }
  else {
    return m;
  }
}


RE* REContext::Read_charset(istream& is)
{
  ASSERT(is.peek() == '[');
  is.get(); //skip the '['
  Char c = is.get();
  BitSet  charset;
  bool negate = false;
  if (c == '^') {
    negate = true;
    c = is.get();
  }
  while (c != EOF && c != ']') {
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


RE* REContext::Read_term(istream& is)
{
  char c = (char)is.peek();
  if (c == '(') {
    is.get();
    RE* re = ReadRE(is);
    if (is.peek() != ')')
      ErrorReadRE("Expected ')' !");
    else
      is.get();
    return re;
  }

  if (c == '[') {
    return Read_charset(is);
  }

  if (c == '{') {
    return Read_macro(is);
  }

  if (c == '\\') { // escaped char
    is.get();
    c = (char)is.peek();
    char* pat = "rnt |*+?([{\\\"";
    char* pos = strchr(pat, c);
    if (pos == 0) {
      char buf[30];
      sprintf(buf, "Invalid escape sequence '\\%c' !", c);
      ErrorReadRE(buf);
    }
    else if (pos - pat < 3) {
      c = ("\r\n\t\ ")[pos - pat];
    }
  }

  is.get();
  return new RE_char(c);
}


RE* REContext::Read_iterats(istream& is)
{
  RE* re = Read_term(is);
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


RE* REContext::Read_concats(istream& is)
{
  RE* re = Read_iterats(is);
  int c;
  while (EOF != (c = is.peek()) && !strchr("|) \r\n\t", c)) {
    re = new RE_concat(re, Read_iterats(is));
  }
  return re;
}


RE* REContext::Read_alterns(istream& is)
{
  RE* re = Read_concats(is);
  while ('|' == is.peek()) {
    is.get();
    re = new RE_altern(re, Read_concats(is));
  }
  return re;
}


RE* REContext::ReadRE(istream& is)
{
  return Read_alterns(is);
}
