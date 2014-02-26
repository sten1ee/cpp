#include "pg/LALR.h"
#include <stdio.h>

LR1_spot::Set  LALR_table::copy_spots(LR1_spot::Set& srcSpots)
{
  LR1_spot::Set  spots(srcSpots.size(), NO_AUTO_INCREMENT);
  for (int i = 0; i < srcSpots.size(); i++) {
    LR1_spot* srcSpot = srcSpots[i];
    Term::Set laTerms(!OWNS_DATA);
    for (int j = 0; j < srcSpot->laTerms.size(); j++) {
      laTerms.insertBack(srcSpot->laTerms[j]);
    }
    LR1_spot* newSpot = new LR1_spot(srcSpot->prod, srcSpot->rhsPos, laTerms);
    newSpot->lr_mode = false;  // to eliminate laTerms when comparing
    spots.insertBack(newSpot);
  }
  return  spots;
}


void  LALR_table::reduce_states(LR1_table& lr1_table, ProgressBar& pb)
{
  // init progress bar:
  pb.start("Reducing LR1 to LALR1 states");

  // factorize & map all 'LR1' to 'LALR' states
  for (int i = 0; i < lr1_table.nStates(); i++) {
    LR1_state*     orig_state = lr1_table.state(i);
    LR1_spot::Set  orig_spots = copy_spots(orig_state->spots);
    // search for reduced state that macthes:
    LR1_state* redu_state;
    size_t k = states.indexOf(&orig_spots);
    if (k == NOT_FOUND) {
      // no match; create new:
      redu_state = new LR1_state(id_to_state.size(), orig_spots);
      id_to_state.push(redu_state);
      states.insert(redu_state);
    }
    else {
      // match exists:
      redu_state = states[k];
      // merge look ahead terms:
      LR1_spot::Set& redu_spots = redu_state->spots;
      for (int i = orig_spots.size(); 0 < i--; ) {
        ASSERT(orig_spots[i]->prod   == redu_spots[i]->prod);
        ASSERT(orig_spots[i]->rhsPos == redu_spots[i]->rhsPos);
        Term::Set& impSet = redu_spots[i]->laTerms;
        Term::Set& expSet = orig_spots[i]->laTerms;
        impSet = impSet | expSet;
      }
    }
    // make a link from the original state to the reduced one:
    orig_state->redu_state = redu_state;

    // merge actions:
    for (int j = 0; j < orig_state->actions.size(); ++j) {
      LR1_action* orig_action = orig_state->actions[j];
      Term* laTerm = orig_action->laTerm;
      // find the action on laTerm:
      LR1_action* redu_action;
      size_t k = redu_state->actions.indexOf(laTerm);
      if (k == NOT_FOUND) {
        redu_action = new LR1_action(laTerm);
        redu_state->actions.insert(redu_action);
        // the actual (lalr) shiftState may not be created yet
        // so save a link to the original to rebind later:
        redu_action->shiftState = orig_action->shiftState;
      }
      else {
        redu_action = redu_state->actions[k];
      }

      redu_action->reduceProds =
          redu_action->reduceProds | orig_action->reduceProds;
      redu_action->accept = redu_action->accept || orig_action->accept;
    }
    // display progress information:
    char  status[32];
    sprintf(status, "%d/%d", i+1, lr1_table.nStates());
    pb.show(status);
  }
  pb.finish(); //clear progress bar

  // init progress bar:
  pb.start("Rebinding LALR1 states");
  // rebind each reduced action's shiftState to the reduced state
  // and detect available conflicts:
  for (int l = states.size(); 0 < l--; ) {
    LR1_action::Set& actions = states[l]->actions;
    for (int a = actions.size(); 0 < a--; ) {
      LR1_action* action = actions[a];
      if (action->shiftState) {
        action->shiftState = action->shiftState->redu_state;
      }
      if (action->has_conflict()) {
        int conflict = action->resolve_conflict();
        if (conflict) {
          conflicts.push(new LR1_conflict(states[l], action, conflict));
        }
      }
    }
    char  status[32];
    sprintf(status, "%d/%d", states.size() - l, states.size());
    pb.show(status);
  }
  pb.finish(); //clear progress bar
}


LALR_table::LALR_table(LR1_table& lr1_table, ProgressBar& pb)
{
  reduce_states(lr1_table, pb);
}

