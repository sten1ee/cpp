#include "fa.h"
#include "faintern.h"
#include "tgroups.h"
#include <strstream.h>
#include <ctype.h>


void  PrintStateSig(const State* s, ostream& os)
{
  os << s->id();
}


void  PrintExtStateSig(const State* s, ostream& os)
{
  const MultiState* ms = SAFE_CAST(s, const MultiState);
  if (ms) {
    os << " {";
    for (int i=0; i < ms->states.size(); i++) {
      if (i) {
        os << ", ";
      }
      PrintStateSig(ms->states[i], os);
    }
    os << "}";
  }
}


void  PrintChar(Char c, ostream& os)
{
  if (c == '\\') {
    os << "\\\\";
  }
  else if (0 <= c && c < 256 && isprint(c)) {
    os << char(c);
  }
  else if (c == EPS_CHAR) {
    os << "\\eps";
  }
  else {
    os << "\\x" << hex << (int)c << dec;
  }
}


void  PrintTransition(const Transition& t, ostream& os)
{
  os << "\t";
  PrintChar(t.charr, os);
  os << "\t";
  PrintStateSig(t.state, os);
}


void  PrintState(const State* s, ostream& os)
{
  char buf[100];
  ostrstream oss(buf, sizeof(buf));
  PrintStateSig(s, oss);
  oss << " ";

  if (s->is_start())
    oss << "S";
  else
    oss << " ";

  State::FinalCodesIter fc = s->final_codes().begin();
  if (!s->final_codes().empty()) {
    oss << "F(" << (*fc).value();
    while (++fc != s->final_codes().end()) {
      oss << "," << (*fc).value();
    }
    oss << ")";
  }
  else {
    oss << "  ";
  }
  oss << '\0';

  int pcount = oss.pcount();
  os << oss.str();
  for (int i=pcount; i < 20; ++i) {
    os << '-';
  }
  PrintExtStateSig(s, os);
  os << "\n";

  const State::Transitions& transitions = s->transitions();
  for (State::TransitionsIter ti  = transitions.begin(), te = transitions.end();
                              ti != te;
                            ++ti) {
    PrintTransition(*ti, os);
    os << "\n";
  }
}


void  PrintFA(const FA* fa, ostream& os)
{
  os << "Automaton " << fa->name() << "\n";
  const FA::States& states = fa->states();
  for (FA::StatesIter si = states.begin(); si != states.end(); ++si) {
    PrintState(*si, os);
  }
}


void  PrintGroup(const TGroup& group, ostream& os)
{
/* -- The following commented code is for verbose group printing --
  os << "Group {";
  const CharSet& mask = group.mask();
  for (CharSet::const_iterator mi  = mask.begin(), me = mask.end();
                               mi != me;
                             ++mi) {
    PrintChar(*mi, os);
  }
  os << "/";
*/
  const CharSet& body = group.body();
  for (CharSet::const_iterator bi  = body.begin(), be = body.end();
                               bi != be;
                             ++bi) {
    PrintChar(*bi, os);
  }
/* -- The following commented code is for verbose group printing --
  os << "}\n\t";
  const TGroup::Id::vector& ids = group.ids();
  for (TGroup::Id::vector::const_iterator ii  = ids.begin(), ie = ids.end();
                                          ii != ie;
                                        ++ii) {
    const TGroup::Id& id = *ii;
    os << "[" << id.src << " -> " << id.dst << "] ";
  }
  os << '\n';
*/
}

void  PrintGroups(const TGroupMgr& groupMgr, ostream& os)
{
  const TGroup::vector& groups = groupMgr.groups();
  for (TGroup::vector::const_iterator gi  = groups.begin(), ge = groups.end();
                                      gi != ge;
                                    ++gi) {
    PrintGroup(**gi, os);
  }
}
