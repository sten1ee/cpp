#include "pg/re.h"
#include "pg/fa.h"

void  RE_concat::create(FA* fa, State* beg, State* end)
{
  State* mid = fa->newState();
  a->create(fa, beg, mid);
  b->create(fa, mid, end);
}


void  RE_altern::create(FA* fa, State* beg, State* end)
{
  a->create(fa, beg, end);
  b->create(fa, beg, end);
}


void  RE_iterat::create(FA* fa, State* beg, State* end)
{
  State* ibeg = fa->newState();
  State* iend = fa->newState();
  a->create(fa, ibeg, iend);
  fa->transition( beg, EPS_CHAR, ibeg);
  fa->transition( beg, EPS_CHAR,  end);
  fa->transition(iend, EPS_CHAR,  end);
  fa->transition(iend, EPS_CHAR, ibeg);
}


void  RE_plusit::create(FA* fa, State* beg, State* end)
{
  State* ibeg = fa->newState();
  State* iend = fa->newState();
  a->create(fa, ibeg, iend);
  fa->transition( beg, EPS_CHAR, ibeg);
  fa->transition(iend, EPS_CHAR,  end);
  fa->transition(iend, EPS_CHAR, ibeg);
}


void  RE_option::create(FA* fa, State* beg, State* end)
{
  a->create(fa, beg, end);
  fa->transition(beg, EPS_CHAR, end);
}


void  RE_char::create(FA* fa, State* beg, State* end)
{
  fa->transition(beg, charr, end);
}


void  RE_charset::create(FA* fa, State* beg, State* end)
{
  for (BitSet::member c  = chars.firstMember();
                      c != BitSet::NO_SUCH_MEMBER;
                      c  = chars.nextMember(c)) {
    fa->transition(beg, (Char)c, end);
  }
}
