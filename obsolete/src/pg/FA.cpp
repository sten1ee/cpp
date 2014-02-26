#include "pg/fa.h"
#include "pg/faintern.h"
#include "pg/re.h"
#include "util/assert.h"
#define SET sorted_vector
#include "util/setoper.h"
#include <stdio.h>


// Computes epsilon-transitions closure;
// If S is a state set then
// 1) Closure(S) contains S as a subset;
// 2) for each state s1 in Closure(S)
//      for each epsilon transition s1-->s2
//        s2 is in Closure(S) also;
// 3) Closure(S) is the minimal set satisfying 1) & 2)
void  Closure(StateSet& stateSet)
{
  State::vector stateVector(!OWNS_DATA);
  for (int i = stateSet.size(); 0 < i--; ) {
    stateVector.push(stateSet[i]);
  }
  while (!stateVector.empty()) {
    State* state = stateVector.pop();
    for (int j = state->eps_transitions.size(); 0 < j--; ) {
      State* newState = state->eps_transitions[j].state;
      if (stateSet.insert_unique(newState)) {
        stateVector.push(newState);
      }
    }
  }
}


// Goto(S, c) = { s | exists s0 in S and transition s0-c->s }
// The function below computes {Goto(S, c) | c in Char}
// (i.e. all Goto(S, ?) are computed at once)
GotoTransitionSet  Goto(StateSet& states)
{
  GotoTransitionSet  gtSet(OWNS_DATA);
  GotoTransition     gtTemp;
  for (int i = states.size(); 0 < i--; ) {
    State* state = states[i];
    for (int j = state->transitions.size(); 0 < j--; ) {
      Transition& t = state->transitions[j];
      gtTemp.charr = t.charr;
      GotoTransitionSet::finger k = gtSet.find(&gtTemp);
      if (!k) {
        gtSet.insert(k, new GotoTransition(t.charr));
      }
      (*k)->states.insert_unique(t.state);
    }
  }
  return gtSet;
}


void  FA::transition(State* beg, Char charr, State* end)
{
  Transition  res(charr, end);
  if (charr == EPS_CHAR)
    beg->eps_transitions.push(res);
  else
    beg->transitions.push(res);
}


State* FA::regState(State* s)
{
  s->id    = states.size();
  states.push(s);
  return s;
}


State* FA::newState()
{
  return regState(new State());
}


void  FA::swapStates(int s1, int s2)
{
  if (s1 != s2) {
    State* temp = states[s1];
    (states[s1] = states[s2])->id = s1;
    (states[s2] = temp      )->id = s2;
  }
}


void  NFA_join_RE(FA* nfa, RE* re, int finalCode)
{
  State* start = nfa->newState();
  State* final = nfa->newState();

  start->code = State::START_FLAG;
  final->code = State::FINAL_MASK & finalCode;
  ASSERT(final->code == finalCode);

  re->create(nfa, start, final);
}


int  GetFinalCode(const MultiState* ms, const StateSet& ssFinals)
{
  StateSet  msFinals = *ms & ssFinals;
  int code = 0;
  for (int i = msFinals.size(); 0 < i--; ) {
    if (code < msFinals[i]->finalCode()) {
      code = msFinals[i]->finalCode();
    }
  }
  return code;
}


FA* NFA_to_MDFA(FA* nfa)
{
  StateSet  ssStarts(!OWNS_DATA);
  StateSet  ssFinals(!OWNS_DATA);
  for (int i = nfa->nStates(); 0 < i--; ) {
    if (nfa->state(i)->isStart()) {
      ssStarts.insert(nfa->state(i));
    }
    if (nfa->state(i)->finalCode()) {
      ssFinals.insert(nfa->state(i));
    }
  }
  Closure(ssStarts);

  char* mdfaName = new char[8 + strlen(nfa->name())];
  sprintf(mdfaName, "MDFA (%s)", nfa->name());
  FA*   mdfa = new FA(mdfaName, OWNS_DATA);

  MultiState* msStart = new MultiState(ssStarts);
  msStart->code = State::START_FLAG | GetFinalCode(msStart, ssFinals);
  mdfa->regState(msStart);

  StateSet  mssAll(!OWNS_DATA);
  mssAll.insert(msStart);

  for (int m = 0; m < mdfa->nStates(); m++) {
    MultiState* msFrom = SURE_CAST(mdfa->state(m), MultiState);
    GotoTransitionSet  gtSet = Goto(*msFrom);
    for (int i = gtSet.size(); 0 < i--; ) {
      GotoTransition* gt = gtSet[i];
      Closure(gt->states);
      MultiState* msTo = new MultiState(gt->states);
      StateSet::finger k = mssAll.find(msTo);
      if (!k) {
        msTo->code = GetFinalCode(msTo, ssFinals);
        mssAll.insert(k, msTo);
        mdfa->regState(msTo);
      }
      else {
        delete msTo;
        msTo = SURE_CAST(*k, MultiState);
      }
      msFrom->transitions.push(Transition(gt->charr, msTo));
    }
  }
  return  mdfa;
}


FA* MDFA_to_DFA(FA* mdfa)
{
  char* dfaName = new char[7 + strlen(mdfa->name())];
  sprintf(dfaName, "DFA (%s)", mdfa->name());
  FA*   dfa = new FA(dfaName, OWNS_DATA);

  for (int s = mdfa->nStates(); 0 < s--; ) {
    dfa->newState();
  }

  for (int i = mdfa->nStates(); 0 < i--; ) {
    ASSERT(mdfa->state(i)->id == i);
    MultiState* msFrom = SURE_CAST(mdfa->state(i), MultiState);
    State*      sFrom = dfa->state(i);
    sFrom->code = msFrom->code;
    for (int ti = msFrom->transitions.size(); 0 < ti--; ) {
      Transition& t = msFrom->transitions[ti];
      dfa->transition(sFrom,
                      t.charr,
                      dfa->state(t.state->id));
    }
  }
  return  dfa;
}


CharSet  GetTransChars(const StateSet& states)
{
  CharSet  transChars;
  // collect all transition chars that go out of this group:
  for (int si = states.size(); 0 < si--; ) {
    State* state = states[si];
    for (int ti = state->transitions.size(); 0 < ti--; ) {
      Transition& t = state->transitions[ti];
      transChars.insert_unique(t.charr);
    }
  }
  return  transChars;
}


State* TransState(State* state, Char charr)
{
  for (int ti = state->transitions.size(); 0 < ti--; ) {
    Transition& t = state->transitions[ti];
    if (t.charr == charr) {
      return t.state;
    }
  }
  return 0;
}


FA* DFA_to_OMDFA(FA* dfa)
{
  // a 'group' is a set of equivalent states;
  // initially there is a group for each different final code;
  // during the 'one char test' a group is likely to split
  // into two or more groups;
  // as the number of groups is supposed to grow dinamically
  // we assign each group an unique gid (group id) begining from 0;
  // sid2gid[i] - the gid of state[i]'s group:
  typedef vector< vector_traits< int > >  GidVector;
  GidVector  sid2gid(dfa->nStates(), NO_INCREMENT);
  // groups[i] - the group with gid i
  vector< vector_traits_ptr< StateSet > >  groups(OWNS_DATA);
  // valid gids are in range [0, groups.size()):
  const int NO_GID  = -1; //undefined gid
  const int ERR_GID = -2; //the gid of the error state group

  // define the initial groups:
  for (int i = 0; i < dfa->nStates(); i++) {
    int fc = dfa->state(i)->finalCode();
    int gid;
    for (gid = groups.size(); 0 < gid--; ) {
      if (fc == (*groups[gid])[0]->finalCode()) {
        // a group for this final code already exists !
        break;
      }
    }
    if (gid < 0) {
      // create a new group for this final code:
      gid = groups.size();
      groups.push(new StateSet(!OWNS_DATA));
    }
    groups[gid]->insert(dfa->state(i));
    sid2gid.push(gid);
  }

  int oldSize = 0;
  // while some group(s) has split during the last pass:
  while (groups.size() != oldSize) {
    oldSize = groups.size();
    // for each group:
    for (int gid = groups.size(); 0 < gid--; ) {
      StateSet& group = *groups[gid];
      if (group.size() == 1) {
        continue; // as it can not split !
      }
      CharSet  transChars = GetTransChars(group);
      GidVector  toGids(dfa->nStates(), NO_INCREMENT);
      for (int i = dfa->nStates(); 0 < i--; ) {
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
          State* stateTo = TransState(state, c);
          int    toGid = stateTo ? sid2gid[stateTo->id] : ERR_GID;
          toGids[state->id] = toGid;
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
            int toGid = toGids[state->id];
            if (toGid != toGidOld) {
              if (newGids[toGid] == NO_GID) {
                // the first occurance of a new group:
                StateSet* newGroup = new StateSet(!OWNS_DATA);
                newGids[toGid] = groups.size();
                groups.push(newGroup);
              }
              sid2gid[state->id] = newGids[toGid];
              group.erase(group.begin() + si);
              groups[newGids[toGid]]->insert(state);
            }
          }
          break; // leave this group (as it has split) and test the next
        }
      }
    }
  }

  char* omdfaName = new char[9 + strlen(dfa->name())];
  sprintf(omdfaName, "OMDFA (%s)", dfa->name());
  FA*   omdfa = new FA(omdfaName, OWNS_DATA);

  for (int gid = 0; gid < groups.size(); gid++) {
    omdfa->regState(new MultiState(*groups[gid]));
  }
  for (int gi = groups.size(); 0 < gi--; ) {
    State* state = omdfa->state(gi);
    State* sub = (*SURE_CAST(state, MultiState))[0];
    for (int ti = sub->transitions.size(); 0 < ti--; ) {
      Char  c = sub->transitions[ti].charr;
      State* subTo = sub->transitions[ti].state;
      omdfa->transition(state, c, omdfa->state(sid2gid[subTo->id]));
    }
    state->code = sub->finalCode();
  }
  for (int si = dfa->nStates(); 0 < si--; ) {
    if (dfa->state(si)->isStart()) {
      int gid = sid2gid[dfa->state(si)->id];
      omdfa->state(gid)->code |= State::START_FLAG;
      // for convenience (when viewing the automaton)
      // set the start state to be the first state:
      omdfa->swapStates(0, gid);
    }
  }
  return omdfa;
}


int  State::compareTo(const State* s) const
{
  if (id < s->id) {
    return -1;
  }
  if (id > s->id) {
    return +1;
  }
  return 0; //equal
}



int  MultiState::compareTo(const State* other) const
{
  const StateSet& x = *this;
  const StateSet& y = *SURE_CAST(other, const MultiState);

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
