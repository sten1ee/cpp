#include "re.h"
#include "fa.h"

void  RE_concat::create(FA* fa, State* beg, State* end) const
{
  State* mid = fa->new_state();
  a->create(fa, beg, mid);
  b->create(fa, mid, end);
}


void  RE_altern::create(FA* fa, State* beg, State* end) const
{
  a->create(fa, beg, end);
  b->create(fa, beg, end);
}


void  RE_iterat::create(FA* fa, State* beg, State* end) const
{
  State* ibeg = fa->new_state();
  State* iend = fa->new_state();
  a->create(fa, ibeg, iend);
  fa->transition( beg, EPS_CHAR, ibeg);
  fa->transition( beg, EPS_CHAR,  end);
  fa->transition(iend, EPS_CHAR,  end);
  fa->transition(iend, EPS_CHAR, ibeg);
}


void  RE_plusit::create(FA* fa, State* beg, State* end) const
{
  State* ibeg = fa->new_state();
  State* iend = fa->new_state();
  a->create(fa, ibeg, iend);
  fa->transition( beg, EPS_CHAR, ibeg);
  fa->transition(iend, EPS_CHAR,  end);
  fa->transition(iend, EPS_CHAR, ibeg);
}


void  RE_option::create(FA* fa, State* beg, State* end) const
{
  a->create(fa, beg, end);
  fa->transition(beg, EPS_CHAR, end);
}


void  RE_char::create(FA* fa, State* beg, State* end) const
{
  fa->transition(beg, charr, end);
}


void  RE_charset::create(FA* fa, State* beg, State* end) const
{
  for (BitSet::member c  = chars.firstMember();
                      c != BitSet::NO_SUCH_MEMBER;
                      c  = chars.nextMember(c)) {
    fa->transition(beg, (Char)c, end);
  }
}

//----------------------------------------------------------------------------
RE* RE_concat::clone() const
{
  return new RE_concat(a->clone(), b->clone());
}


RE* RE_altern::clone() const
{
  return new RE_altern(a->clone(), b->clone());
}


RE* RE_iterat::clone() const
{
  return new RE_iterat(a->clone());
}


RE* RE_plusit::clone() const
{
  return new RE_plusit(a->clone());
}


RE* RE_option::clone() const
{
  return new RE_option(a->clone());
}


RE* RE_char::clone() const
{
  return new RE_char(charr);
}


RE* RE_charset::clone() const
{
  BitSet cloned_chars = chars.clone();
  return new RE_charset(cloned_chars);
}
