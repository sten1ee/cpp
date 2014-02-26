#include "fa.h"
#include "re.h"
#include "faintern.h"
#include "util/assert.h"
#include <stdio.h>

typedef State::Transitions  Transitions;
typedef Transitions::const_iterator TransitionsIter;

// Computes epsilon-transitions closure;
// If S is a state set then
// 1) Closure(S) contains S as a subset;
// 2) for each state s1 in Closure(S)
//      for each epsilon transition s1-->s2
//        s2 is in Closure(S) also;
// 3) Closure(S) is the minimal set satisfying 1) & 2)
void  MultiState::Closure(State::set& states)
{
  State::vector stateStack(!OWNS_DATA);
  for (State::set::iterator i  = states.begin(), ie = states.end();
                            i != ie;
                          ++i) {
    stateStack.push(*i);
  }
  while (!stateStack.empty()) {
    const Transitions& transitions = stateStack.pop()->transitions();
    for (TransitionsIter ti  = transitions.begin(), te = transitions.end();
                         ti != te && (*ti).charr == EPS_CHAR;
                       ++ti) {
      State* toState = (*ti).state;
      if (states.insert_unique(toState)) {
        stateStack.push(toState);
      }
    }
  }
}


// Goto(S, c) = { s | exists s0 in S and transition s0-c->s }
// The function below computes {Goto(S, c) | c in Char}
// (i.e. all Goto(S, ?) are computed at once)
GotoTransition::set  MultiState::Goto(const State::set& states)
{
  GotoTransition::set  gtSet(OWNS_DATA);
  for (State::set::const_iterator si  = states.begin(), se = states.end();
                                  si != se;
                                ++si) {
    const Transitions& transitions =  (*si)->transitions();
    for (TransitionsIter ti  = transitions.begin(), te = transitions.end();
                         ti != te;
                       ++ti) {
      const Transition& t = *ti;
      if (t.charr == EPS_CHAR) {
        continue;
      }
      GotoTransition::set::finger k = gtSet.find(t.charr);
      if (!k) {
        gtSet.insert(k, new GotoTransition(t.charr));
      }
      (*k)->states.insert_unique(t.state);
    }
  }
  return gtSet;
}


void  MultiState::genFinalCodes(FinalCode::policy_t fcp)
{
  for (State::set::const_iterator si  = states.begin(), se = states.end();
                                  si != se;
                                ++si) {
    _final_codes |= (*si)->final_codes();
  }
  if (fcp == FinalCode::SINGLE && _final_codes.size() > 1) {
    _final_codes.erase(++_final_codes.begin(), _final_codes.end());
  }
}


CharSet  MultiState::GetTransChars(const State::set& states)
{
  CharSet  transChars;
  // collect all transition chars that go out of this group:
  for (State::set::const_iterator si  = states.begin(), se = states.end();
                                  si != se;
                                ++si) {
    const Transitions& transitions = (*si)->transitions();
    for (TransitionsIter ti  = transitions.begin(), te = transitions.end();
                         ti != te;
                       ++ti) {
      transChars.insert_unique((*ti).charr);
    }
  }
  return  transChars;
}


int  MultiState::compareTo(const State* other) const
{
  const State::set& x = states;
  const State::set& y = SURE_CAST(other, const MultiState)->states;

  if (x.size() != y.size()) {
    return x.size() < y.size() ? -1 : 1;
  }
  for (int i=0; i < x.size(); i++) {
    int res = x[i]->compareTo(y[i]);
    if (res) {
      return res;
    }
  }
  return 0; //equal
}


void  FA::transition(State* beg, Char charr, State* end)
{
  beg->_transitions.insert(Transition(charr, end));
}


State* FA::reg_state(State* s)
{
  s->_id = _states.size();
  _states.push(s);
  return s;
}


State* FA::new_state()
{
  return reg_state(new State());
}


void  FA::swap_states(int s1, int s2)
{
  if (s1 != s2) {
    State* temp  = _states[s1];
    (_states[s1] = _states[s2])->_id = s1;
    (_states[s2] = temp       )->_id = s2;
  }
}


void  FA::NFA_join_RE(FA& nfa, RE* re, FinalCode fc)
{
  State* start = nfa.new_state();
  State* final = nfa.new_state();

  start->_is_start = true;
  final->_final_codes.insert_back(fc);

  re->create(&nfa, start, final);
}


FA* FA::NFA_to_MDFA(FA& nfa, FinalCode::policy_t fcp)
{
  char* mdfaName = new char[8 + strlen(nfa.name())];
  sprintf(mdfaName, "MDFA (%s)", nfa.name());
  FA*   mdfa = new FA(mdfaName, OWNS_DATA);

  State::set  ssStarts(!OWNS_DATA);
  for (int i = nfa.n_states(); 0 < i--; ) {
    if (nfa.state(i)->is_start()) {
      ssStarts.insert(nfa.state(i));
    }
  }
  MultiState::Closure(ssStarts);
  MultiState* msStart = new MultiState(ssStarts);
  msStart->_is_start = true;
  msStart->genFinalCodes(fcp);

  mdfa->reg_state(msStart);

  State::set  mssAll(!OWNS_DATA);
  mssAll.insert(msStart);

  for (int m = 0; m < mdfa->n_states(); m++) {
    MultiState* msFrom = SURE_CAST(mdfa->state(m), MultiState);
    GotoTransition::set  gtSet = MultiState::Goto(msFrom->states);
    for (GotoTransition::set::iterator gti  = gtSet.begin(), gte = gtSet.end();
                                       gti != gte;
                                     ++gti) {
      GotoTransition* gt = *gti;
      MultiState::Closure(gt->states);
      MultiState* msTo = new MultiState(gt->states);
      State::set::finger k = mssAll.find(msTo);
      if (!k) {
        msTo->genFinalCodes(fcp);
        mssAll.insert(k, msTo);
        mdfa->reg_state(msTo);
      }
      else {
        delete msTo;
        msTo = SURE_CAST(*k, MultiState);
      }
      msFrom->_transitions.insert(Transition(gt->charr, msTo));
    }
  }
  return  mdfa;
}


FA* FA::MDFA_to_DFA(FA& mdfa)
{
  char* dfaName = new char[7 + strlen(mdfa.name())];
  sprintf(dfaName, "DFA (%s)", mdfa.name());
  FA*   dfa = new FA(dfaName, OWNS_DATA);

  for (int s = mdfa.n_states(); 0 < s--; ) {
    dfa->new_state();
  }

  for (int i = mdfa.n_states(); 0 < i--; ) {
    ASSERT(mdfa.state(i)->id() == i);
    State* msFrom = mdfa.state(i);
    State* sFrom  = dfa->state(i);
    sFrom->_final_codes = msFrom->final_codes();
    sFrom->_is_start    = msFrom->is_start();
    const Transitions& transitions = msFrom->transitions();
    for (TransitionsIter ti  = transitions.begin(), te = transitions.end();
                         ti != te;
                       ++ti) {
      const Transition& t = *ti;
      dfa->transition(sFrom,
                      t.charr,
                      dfa->state(t.state->id()));
    }
  }
  return  dfa;
}


FA* FA::DFA_to_OMDFA(FA& dfa)
{
  // a 'group' is a set of equivalent states;
  // initially there is a group for each different final code;
  // during the 'one char test' a group is likely to split
  // into two or more groups;
  // as the number of groups is supposed to grow dinamically
  // we assign each group an unique gid (group id) begining from 0;
  // sid2gid[i] - the gid of state[i]'s group:
  typedef vector< vector_traits< int > >  GidVector;
  GidVector  sid2gid(dfa.n_states(), NO_INCREMENT);
  // groups[i] - the group with gid i
  vector< vector_traits_ptr< State::set > >  groups(OWNS_DATA);
  // valid gids are in range [0, groups.size()):
  const int NO_GID  = -1; //undefined gid
  const int ERR_GID = -2; //the gid of the error state group

  // define the initial groups:
  for (int i = 0; i < dfa.n_states(); i++) {
    const State::FinalCodes& fc = dfa.state(i)->final_codes();
    int gid;
    for (gid = groups.size(); 0 < gid--; ) {
      if (is_equal(fc, (*groups[gid])[0]->final_codes())) {
        // a group for this final code already exists !
        break;
      }
    }
    if (gid < 0) {
      // create a new group for this final code:
      gid = groups.size();
      groups.push(new State::set(!OWNS_DATA));
    }
    groups[gid]->insert(dfa.state(i));
    sid2gid.push(gid);
  }

  int oldSize = 0;
  // while some group(s) has split during the last pass:
  while (groups.size() != oldSize) {
    oldSize = groups.size();
    // for each group:
    for (int gid = groups.size(); 0 < gid--; ) {
      State::set& group = *groups[gid];
      if (group.size() == 1) {
        continue; // as it can not split !
      }
      CharSet  transChars = MultiState::GetTransChars(group);
      GidVector  toGids(dfa.n_states(), NO_INCREMENT);
      for (int i = dfa.n_states(); 0 < i--; ) {
        toGids.push(NO_GID);
      }
      bool  hasSplit = false;
      // for each char in transChar:
      for (int ci = transChars.size(); 0 < ci--; ) {
        Char c = transChars[ci];
        int  toGidOld = NO_GID;
        // for each state in the group:
        for (int si = group.size(); 0 < si--; ) {
          State* state = group[si];
          // perform the 'one char test':
          State* stateTo = state->trans_state(c);
          int    toGid = stateTo ? sid2gid[stateTo->id()] : ERR_GID;
          toGids[state->id()] = toGid;
          if (toGidOld != NO_GID) {
            if (toGidOld != toGid) {
              hasSplit = true;
            }
          }
          if (toGidOld != ERR_GID) {
            toGidOld = toGid; // preferentially hold ERR_GID (if encountered)
          }
        }
        if (hasSplit) {
          // newGids[toGids[state->id]] -> the (new) group of state
          GidVector  newGids(groups.size(), NO_INCREMENT);
          for (int gi = groups.size(); 0 < gi--; ) {
            newGids.push(NO_GID);
          }
          // determine the (new) gid of each state of the group:
          for (int si = group.size(); 0 < si--; ) {
            State* state = group[si];
            int toGid = toGids[state->id()];
            if (toGid != toGidOld) {
              if (newGids[toGid] == NO_GID) {
                // the first occurance of a new group:
                State::set* newGroup = new State::set(!OWNS_DATA);
                newGids[toGid] = groups.size();
                groups.push(newGroup);
              }
              sid2gid[state->id()] = newGids[toGid];
              group.erase(group.begin() + si);
              groups[newGids[toGid]]->insert(state);
            }
          }
          break; // leave this group (as it has split) and test the next
        }
      }
    }
  }

  char* omdfaName = new char[9 + strlen(dfa.name())];
  sprintf(omdfaName, "OMDFA (%s)", dfa.name());
  FA*   omdfa = new FA(omdfaName, OWNS_DATA);

  for (int gid = 0; gid < groups.size(); gid++) {
    omdfa->reg_state(new MultiState(*groups[gid]));
  }
  for (int gi = groups.size(); 0 < gi--; ) {
    State* state = omdfa->state(gi);
    State* sub = SURE_CAST(state, MultiState)->states[0];

    const Transitions& transitions = sub->transitions();
    for (TransitionsIter ti  = transitions.begin(), te = transitions.end();
                         ti != te ;
                       ++ti) {
      Transition t = *ti;
      omdfa->transition(state, t.charr, omdfa->state(sid2gid[t.state->id()]));
    }
    state->_is_start    = sub->is_start();
    state->_final_codes = sub->_final_codes;
  }
  for (int si = dfa.n_states(); 0 < si--; ) {
    if (dfa.state(si)->is_start()) {
      int gid = sid2gid[dfa.state(si)->id()];
      omdfa->state(gid)->_is_start = true;
      // for convenience (when viewing the automaton)
      // set the start state to be the first state:
      omdfa->swap_states(0, gid);
      break; // as there is surely only one start state.
    }
  }
  return omdfa;
}


void  FA::DFA_to_CDFA(FA& dfa)
{
  for (int pget = 0, pput = 1; pget < pput; ++pget) {
    State* state = dfa.state(pget);
    const State::Transitions& trans = state->transitions();
    for (State::TransitionsIter ti  = trans.begin(), te = trans.end();
                                ti != te;
                              ++ti) {
      State* toState = (*ti).state;
      if (pput <= toState->id()) {
        if (pput < toState->id()) {
          // fprintf(stderr, "%d:%c swaps[%d - %d]\n", pget, (*ti).charr, pput, toState->id());
          dfa.swap_states(pput, toState->id());
        }
        pput++;
      }
    }
  }
  char* newName = new char[strlen(dfa.name()) + 8];
  sprintf(newName, "CDFA (%s)", dfa.name());
  delete[] dfa.name();
  dfa._name = newName;
}


FA* FA::NFA_to_ODFA(FA& nfa, FinalCode::policy_t fcp)
{
  FA* mdfa = NFA_to_MDFA(nfa, fcp);

  FA* dfa = MDFA_to_DFA(*mdfa);
  delete mdfa;

  FA* omdfa = DFA_to_OMDFA(*dfa);
  delete dfa;

  FA* odfa = MDFA_to_DFA(*omdfa);
  delete omdfa;

  DFA_to_CDFA(*odfa);

  return odfa;
}
