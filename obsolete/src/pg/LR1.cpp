#include "pg/LR1.h"
#include "util/bitset.h"
#include <stdio.h>

// Closure(I) :
// repeat
//   for any item (A -> a.Xb, z) in I
//     for any w in FIRST(bz)
//       for any prod X -> c
//         I |= {(X -> .c, w)}
// until I does not change
LR1_spot::Set  LR1_table::items_closure(LR0_item::Set& genStack)
{
  Term::Set      after_x(!OWNS_DATA);
  LR0_item::Set  lr0_items(!OWNS_DATA);

  while (!genStack.isEmpty()) {
    ASSERT(after_x.isEmpty());
    LR1_spot* spot = SURE_CAST(genStack.pop(), LR1_spot);
    Term*     x    = spot->isPast() ? 0 : spot->topicalTerm();
    bool      stub = !x || x->isTerminal();
    size_t k = lr0_items.indexOf(spot);
    if (k == NOT_FOUND) {
      lr0_items.insert(spot);
      if (stub) {
        continue; //stubs do not propagate
      }

      int i;  //after_x:
      for (i = spot->rhsPos; ++i < spot->prod->rhsTerms.size(); ) {
        after_x = after_x | spot->prod->rhsTerms[i]->firstSet;
        if (!spot->prod->rhsTerms[i]->nullable) {
          break;
        }
      }
      if (i == spot->prod->rhsTerms.size()) {  //if every term in b is nullable
        after_x = after_x | spot->laTerms;//z is member of first_bz
      }
    }
    else {
      LR1_spot* pro_spot = SURE_CAST(lr0_items[k], LR1_spot);
      if (stub) {
        pro_spot->laTerms = pro_spot->laTerms | spot->laTerms;
        delete spot;
        continue; //stubs do not propagate
      }

      int i;
      for (i = spot->rhsPos; ++i < spot->prod->rhsTerms.size(); ) {
        if (!spot->prod->rhsTerms[i]->nullable) {
          break;
        }
      }
      if (i == spot->prod->rhsTerms.size()) {
        after_x = spot->laTerms - pro_spot->laTerms;
        pro_spot->laTerms = pro_spot->laTerms | after_x;
      }
      else {
        pro_spot->laTerms = pro_spot->laTerms | spot->laTerms;
      }
      delete spot;
    }

    if (!after_x.isEmpty()) {
      for (ProdNode* p = x->prodFirst; p != 0; p = p->prodNext) {
        Term::Set  laTerms(!OWNS_DATA);
        if (p->prodNext == 0) {
          laTerms = after_x;
        }
        else {
          // copy after_x into laTerms:
          for (int i = 0; i < after_x.size(); i++) {
            laTerms.insertBack(after_x[i]);
          }
        }

        size_t k = genStack.indexOf(&LR0_item(p, 0));
        if (k == NOT_FOUND) {
          genStack.insert(new LR1_spot(p, 0, laTerms));
        }
        else {
          LR1_spot* pro_spot = SURE_CAST(genStack[k], LR1_spot);
          pro_spot->laTerms = pro_spot->laTerms | laTerms;
        }
      }
    }
  }

  LR1_spot::Set  lr1_spots(lr0_items.size(), NO_AUTO_INCREMENT);
  for (int i = 0; i < lr0_items.size(); i++) {
    lr1_spots.insertBack(SURE_CAST(lr0_items[i], LR1_spot));
  }
  return lr1_spots;
}


LR1_state* LR1_table::make_state(LR1_spot::Set& spotSet)
{
  size_t  k = states.indexOf(&spotSet);
  if (k == NOT_FOUND) {
    LR1_state* newState = new LR1_state(id_to_state.size(), spotSet);
    id_to_state.push(newState);
    states.insert(newState);
    return newState;
  }
  else {
    return states[k];
  }
}


LR1_action* LR1_state::action_on(Term* term)
{
  LR1_action* action;
  size_t k = actions.indexOf(term);
  if (k == NOT_FOUND) {
    action = new LR1_action(term);
    actions.insert(action);
  }
  else {
    action = actions[k];
  }
  return  action;
}


void  LR1_table::calc_actions(LR1_state* state)
{
  // the actions vector is indexed with term id(s)
  // actions(i) is the action taken on lookahead grammar()->term(i)
  for (int i = 0; i < state->spots.size(); ++i) {
    LR1_spot* spot = state->spots[i];
    if (spot->isPast()) {
      Term::Set& laTerms = spot->laTerms;
      for (int j = 0; j < laTerms.size(); j++) {
        state->action_on(laTerms[j])->addReduce(spot->prod);
      }
    }
    else {
      Term* topTerm = spot->topicalTerm();
      LR1_action* action = state->action_on(topTerm);
      if (topTerm == grammar.term(Grammar::EOF_TERM)) {
        action->accept = true;
      }
      else {
        Term::Set laTerms(spot->laTerms.size(), NO_AUTO_INCREMENT);
        laTerms.ownsData(false);
        for (int j = 0; j < spot->laTerms.size(); j++) {
          laTerms.insertBack(spot->laTerms[j]);
        }
        action->shiftItems.insertBack(
            new LR1_spot(spot->prod, spot->rhsPos + 1, laTerms));
      }
    }
  }
  for (int j = 0; j < state->actions.size(); j++) {
    LR1_action* action = state->actions[j];
    // check for possible Shift-Reduce & Reduce-Reduce conflict(s)
    if (action->has_conflict()) {
      int conflict = action->resolve_conflict();
      if (conflict) {
        conflicts.push(new LR1_conflict(state, action, conflict));
      }
    }
    if (!action->shiftItems.isEmpty()) {
      // form the state to shift to:
      action->shiftState = make_state(items_closure(action->shiftItems));
    }
  }
}


void  LR1_table::calc_states(ProgressBar& pb)
{
  pb.start("Calculating LR1 states");
  char  status[32];
  for (int i = 0; i < id_to_state.size(); i++) {
    sprintf(status, "%d/%d", i+1, id_to_state.size());
    pb.show(status);
    LR1_state* state = id_to_state[i];
    calc_actions(state);
  }
  pb.finish();
}


bool  LR1_action::has_conflict() const
{
  int  nReduces = reduceProds.size() + (accept ? 1 : 0);
  return  (shiftState != 0 || !shiftItems.isEmpty()) && nReduces != 0
      ||  (nReduces > 1);
}


int   LR1_action::resolve_conflict()
{
  // try to resolve any Shift-Reduce or(and) Reduce-Reduce conflict(s);
  // for that purpose determine each reduceProd's precedence and
  // if there is a Shift-Reduce conflict - the laTerm precedence;
  // the action is not resolvable iff some of the above mentioned precedences
  // is undefined (0) or else when two  or more reduceProds are 'top bananas'
  // (i.e. are not preceded by any other prod nor by the shift (if present))

  // find max precedence among all reduceProds:
  int  top_prec = 0;
  for (int j = reduceProds.size(); 0 < j--; ) {
    Prod* prod = reduceProds[j];
    if (prod->prec == 0) {
      // prod has no explicitly assigned %prec so try to deduce:
      prod->prec = prod->deduce_prec();
    }
    if (top_prec < prod->prec) {
      top_prec = prod->prec;
    }
  }

  // check the shift (if any):
  bool  does_shift = shiftState != 0 || !shiftItems.isEmpty();
  if (does_shift) {
    int shift_prec = laTerm->prec;
    if (shift_prec != 0) {
      bool  flush_shift;
      if (shift_prec < top_prec) {      // resolve as reduce:
        flush_shift = true;
      }
      else if (shift_prec > top_prec) { // resolve as shift:
        flush_shift = false;
        top_prec = shift_prec;
      }
      else { // equal precedence so consider associativity:
        switch (laTerm->assoc) {
          case Term::ASSOC_LEFT:  //resolve as reduce
            flush_shift = true;
            break;
          case Term::ASSOC_RIGHT: //resolve as shift
            flush_shift = false;
            top_prec = shift_prec + 1;
            break;
          case Term::ASSOC_NONASSOC: //reslove as error
            flush_shift = true;
            top_prec = shift_prec + 1;
            break;
          default:
            ASSERT(0);
        }
      }
      if (flush_shift) {
        shiftState = 0;
        shiftItems.flush();
        does_shift = false;
      }
    } // endif (shift_prec != 0)
  } //endif (all_def && does_shift)

  // flush all reduceProds that are dominated:
  for (int k = reduceProds.size(); 0 < k--; ) {
    Prod* prod = reduceProds[k];
    if (prod->prec != 0 && prod->prec != top_prec) {
      reduceProds.removeAt(k);
    }
  }

  int conflict = 0;
  if (!reduceProds.isEmpty()) {
    if (does_shift) {
      conflict |= LR1_conflict::SR;
    }
    if (accept) {
      conflict |= LR1_conflict::AR;
    }
    if (reduceProds.size() > 1) {
      conflict |= LR1_conflict::RR;
    }
  }

  return conflict;
}


void  LR1_table::resolve_conflicts()
{
  for (int i = conflicts.size(); 0 < i--; ) {
    int conflict = conflicts[i]->action->resolve_conflict();
    if (conflict) {
      conflicts[i]->type = conflict;
    }
    else {
      conflicts.removeAt(i);
    }
  }
}


LR1_table::LR1_table(Grammar& grammar, ProgressBar& pb)
  : grammar(grammar), id_to_state(!OWNS_DATA)
{
  LR0_item::Set  startItems;
  Term* iniTerm = grammar.term(Grammar::INI_TERM);
  Term* eofTerm = grammar.term(Grammar::EOF_TERM);
  for (ProdNode* p = iniTerm->prodFirst; p != 0; p  = p->prodNext) {
    Term::Set  laTerms(!OWNS_DATA);
    laTerms.insert(eofTerm);
    startItems.insert(new LR1_spot(p, 0, laTerms));
  }

  make_state(items_closure(startItems));
  calc_states(pb);
}


LR1_table::LR1_table()
  : grammar(*(Grammar*)0), id_to_state(!OWNS_DATA)
{
}


int  LR0_item::Traits::Compare(const LR0_item* i, const LR0_item* j)
{
  // Remark: it is possible (and sure faster) to compare bare addresses
  // instead of id(s) as this preserves one memory fetch;
  // I'm not doing it to keep the code clear;
  if (i->prod != j->prod) {
    return  i->prod->id < j->prod->id ? -1 : +1;
  }
  if (i->rhsPos != j->rhsPos) {
    return  i->rhsPos < j->rhsPos ? -1 : +1;
  }
  return 0; //equal
}


int  LR1_spot::Traits::Compare(const LR1_spot* i, const LR1_spot* j)
{
  if (i->prod != j->prod) {
    return  i->prod->id < j->prod->id ? -1 : +1;
  }
  if (i->rhsPos != j->rhsPos) {
    return  i->rhsPos < j->rhsPos ? -1 : +1;
  }

  ASSERT(i->lr_mode == j->lr_mode);

  if (i->lr_mode) {
    const Term::Set& iTerms = i->laTerms;
    const Term::Set& jTerms = j->laTerms;
    if (iTerms.size() != jTerms.size()) {
      return  iTerms.size() < jTerms.size() ? -1 : +1;
    }
    for (int k = 0; k < iTerms.size(); k++) {
      if (iTerms[k] != jTerms[k]) {
        return  iTerms[k]->id < jTerms[k]->id ? -1 : +1;
      }
    }
  }

  return 0; //equal
}


int  LR1_state::Traits::Compare(const LR1_spot::Set* spotSet,
                                const LR1_state*     state)
{
  if (spotSet->size() != state->spots.size()) {
    return  spotSet->size() < state->spots.size() ? -1 : +1;
  }
  for (int i = 0; i < spotSet->size(); i++) {
    int  diff = LR1_spot::Traits::Compare((*spotSet)[i], state->spots[i]);
    if (diff) {
      return diff;
    }
  }
  return 0; //equal
}
