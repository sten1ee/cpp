#include "pg/fa.h"
#include "pg/faintern.h"
#include <iostream.h>
#include <iomanip.h>
#include <ctype.h>


void  PrintStateSig(State* s, ostream& os)
{
  os << s->id;
}


void  PrintExtStateSig(State* s, ostream& os)
{
  MultiState* ms = SAFE_CAST(s, MultiState);
  if (ms) {
    os << " {";
    for (int i=0; i < ms->size(); i++) {
      if (i) {
        os << ", ";
      }
      PrintStateSig((*ms)[i], os);
    }
    os << "}";
  }
}


void  PrintTransition(Transition& t, ostream& os)
{
  os << "\t";
  Char c = t.charr;
  if (c == EPS_CHAR) {
    os << "eps";
  }
  else if (0 <= c && c < 256 && isgraph(c)) {
    os << "'" << char(c) << "'";
  }
  else {
    os << c;
  }
  os << "\t";
  PrintStateSig(t.state, os);
}


void  PrintState(State* s, ostream& os)
{
  //os << "State ";
  PrintStateSig(s, os);
  os << " ";

  if (s->isStart())
    os << "S";
  else
    os << " ";

  if (s->finalCode())
    os << "F" << s->finalCode();
  else
    os << "  ";

  os << " --------------";
  PrintExtStateSig(s, os);
  os << "\n";

  for (int i=0; i < s->transitions.size(); i++) {
    PrintTransition(s->transitions[i], os);
    os << "\n";
  }
  for (int j=0; j < s->eps_transitions.size(); j++) {
    PrintTransition(s->eps_transitions[j], os);
    os << "\n";
  }
}


void  PrintFA(FA* fa, ostream& os)
{
  os << "Automaton " << fa->name() << "\n";
  for (int i=0; i < fa->nStates(); i++) {
    PrintState(fa->state(i), os);
  }
}
