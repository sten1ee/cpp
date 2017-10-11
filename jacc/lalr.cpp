#include "lalr.h"
#include "util/msg_listener.h"

// Appends the contents of the second container at the end of the first
// container. The first container must support the 'push_back' operation.
//
template <class A, class B>
void  push_back(A& a, B& b)
{
  for (typename B::iterator i = b.begin(); i != b.end(); ++i) {
    a.push_back(*i);
  }
}

// Appends the contents of the second container at the end of the first
// container. The first container must support the 'insert_back' operation.
//
template <class A, class B>
void  insert_back(A& a, B& b)
{
  for (typename B::iterator i = b.begin(); i != b.end(); ++i) {
    a.insert_back(*i);
  }
}

// Performs the 'Closure' opereation over the passed state.
// i.e. generates the non-core items and the inner channels.
//
void  LALR_table::make_closure(LALR_state* state)
{
  LALR_item::set     non_core_items(!OWNS_DATA);
  LALR_item::vector  gen_stack(!OWNS_DATA);
  push_back(gen_stack, state->items);

  while (!gen_stack.empty()) {
    LALR_item* item = SURE_CAST(gen_stack.pop(), LALR_item);
    if (item->is_past() || item->topical_term()->is_terminal()) {
      continue;
    }
    Term* x = item->topical_term();
    bool  all_nullables;
    Term::set after_x(!OWNS_DATA);
    {
      int i;
      const Term::array& rhs_terms = item->prod->rhs_terms;
      for (i = item->rhs_pos + 1; i < rhs_terms.size(); ++i) {
        after_x |= rhs_terms[i]->first_set;
        if (!rhs_terms[i]->nullable) {
          break;
        }
      }
      all_nullables = (i == rhs_terms.size());
    }

    for (Term::prods_iterator pi = x->prods_begin(), end = x->prods_end();
                              pi != end; ++pi) {
      Prod* p = *pi;
      LR0_item temp(p, 0);
      LR0_item::set::finger f = non_core_items.find(&temp);
      LALR_item* x_item;
      if (!f) {
        x_item = new LALR_item(p, 0);
        non_core_items.insert(f, x_item);
        gen_stack.push(x_item);
      }
      else {
        x_item = SURE_CAST(*f, LALR_item);
      }
      x_item->new_la_terms |= after_x;
      if (all_nullables && item != x_item) {
        item->items_to_notify.push(x_item);
      }
    }
  }

  ASSERT(state->n_core_items == state->items.size());
  // as each core item < any non core item it is correct
  // and more efficient to use insert_back instead of `|=' :
  insert_back(state->items, non_core_items);
}

// Gets the action object corresponding to the given lookahead.
//
LALR_action* LALR_state::action_on(Term* term)
{
  LALR_action* action;
  LALR_action::set::finger f = actions.find(term);
  if (!f) {
    action = new LALR_action(term);
    actions.insert(f, action);
  }
  else {
    action = *f;
  }
  return  action;
}

// Generates the transition actions (those of the type 'shift T and goto S')
// and the outcoming lookahead propagation channels of the state given.
//
void  LALR_table::calc_transitions(LALR_state* state)
{
  // Phase 1:
  // Fill in the shift-stubs of each action:
  //
  for (LALR_item::set::iterator ii  = state->items.begin();
                                ii != state->items.end(); ++ii) {
    LALR_item* item = SURE_CAST(*ii, LALR_item);
    if (!item->is_past()) {
      Term* top_term = item->topical_term();
      LALR_action* action = state->action_on(top_term);
      if (top_term == grammar->term(Grammar::EOF_TERM)) {
        action->setf(LALR_action::DO_ACCEPT);
      }
      else {
        LALR_stub* stub = new LALR_stub(item->prod, item->rhs_pos + 1);
        stub->item_from = item;
        action->add_stub(stub);
      }
    }
  }

  // Phase 2:
  // Find the actual LALR_state behind each action's LALR_stub set.
  // Construct the appropriate channels.
  //
  for (LALR_action::set::iterator ai  = state->actions.begin();
                                  ai != state->actions.end(); ++ai) {
    LALR_action* action = *ai;
    if (action->has_stubs()) {
      LR0_item::set& stubs = action->get_stubs();
      // form the state to shift to:
      action->shift_state = stubs_to_state(stubs);
      action->setf(LALR_action::DO_SHIFT);
      for (LALR_stub::set::iterator si  = stubs.begin();
                                    si != stubs.end(); ++si) {
        LALR_stub* stub = SURE_CAST(*si, LALR_stub);
        stub->item_from->items_to_notify.push(stub->item_to);
      }
      // dispose of the shift_stubs container:
      action->dispose_of_stubs();
    }
  }
}

// When all states and their transitions & channels are generated
// this method is called to do the lookahead propagation.
//
void  LALR_table::calc_la_terms()
{
  LALR_item::vector  gen_stack(!OWNS_DATA);
  for (int i = 0; i < id_to_state.size(); i++) {
    LALR_state* state = id_to_state[i];
    for (LALR_item::set::iterator j  = state->items.begin();
                                  j != state->items.end(); ++j) {
      LALR_item* item = SURE_CAST(*j, LALR_item);
      if (!item->new_la_terms.empty()) {
        gen_stack.push(item);
      }
    }
  }

  while (!gen_stack.empty()) {
    LALR_item* item = SURE_CAST(gen_stack.pop(), LALR_item);
    for (int i = item->items_to_notify.size(); 0 < i--; ) {
      LALR_item* notify_item = SURE_CAST(item->items_to_notify[i], LALR_item);
      if (!is_subset(item->new_la_terms, notify_item->la_terms)) {
        if (notify_item->new_la_terms.empty()) {
          gen_stack.push(notify_item);
        }
        notify_item->new_la_terms |= item->new_la_terms;
      }
    }
    item->la_terms |= item->new_la_terms;
    item->new_la_terms.flush();
  }
}

// Gets the table state with the specified core - stubs.
// If a state with the specified core already exists it is returned;
// otherwise a new state is created, inserted into the table & returned.
//
LALR_state* LALR_table::stubs_to_state(LALR_stub::set& stubs)
{
  LALR_state::set::finger f = states.find(LALR_state::core(stubs, stubs.size()));
  if (f) {
    LALR_state* state = *f;
    LALR_item::set::iterator i = state->items.begin();
    for (LALR_stub::set::iterator s  = stubs.begin();
                                  s != stubs.end(); ++s, ++i) {
      LALR_stub* stub = SURE_CAST(*s, LALR_stub);
      LALR_item* item = SURE_CAST(*i, LALR_item);
      stub->item_to = item;
    }
    return state;
  }
  else {
    LALR_state* state = new LALR_state(n_states());
    for (LALR_stub::set::iterator i  = stubs.begin();
                                  i != stubs.end(); ++i) {
      LALR_stub* stub = SURE_CAST(*i, LALR_stub);
      LALR_item* item = new LALR_item(stub->prod, stub->rhs_pos);
      stub->item_to = item;
      state->items.insert_back(item);
    }
    state->n_core_items = state->items.size();
    states.insert(f, state);
    id_to_state.push_back(state);
    return state;
  }
}

// When the lookahead sets are generated this method is called
// to calculate the reduction actions of the states.
//
void  LALR_table::calc_reductions()
{
  for (int i = 0; i < n_states(); ++i) {
    LALR_state* state = id_to_state[i];
    for (LALR_item::set::iterator j  = state->items.begin();
                                  j != state->items.end(); ++j) {
      LALR_item* item = SURE_CAST(*j, LALR_item);
      if (item->is_past()) {
        for (Term::set::iterator k  = item->la_terms.begin();
                                 k != item->la_terms.end(); ++k) {
          LALR_action* action = state->action_on(*k);
          action->add_reduce(item->prod);
          if ( action->getf(LALR_action::IS_CONFLICT) != 0
            && action->host_state == 0) {
            // first time that a conflict is detected in this action, so:
            action->host_state = state;
            conflicts.push_back(action);
          }
        }//next term
      }
    }//next item
  }//next state
}

// Iterates through each conflict action calling its resolve_conflics() method.
// Returns the number of conflicts that failed to resolve.
//
int  LALR_table::resolve_conflicts()
{
  int n_unresolved = 0;
  // run conflict resolution:
  for (int i = conflicts.size(); 0 < i--; ) {
    LALR_action* conflict_action = conflicts[i];
    if (!conflict_action->resolve_conflicts()) {
      n_unresolved++;
    }
  }
  return n_unresolved;
}

// Returns the id of the most frequently reduce-by rule for the given state.
// Only rules used more than max_count times are considered.
// As a side-effect the reduce_counts array (of size >= grammar.n_prods())
// holds the reduce-by frequencies of the grammar productions for this state.
//
int  LALR_table::calc_default_reduce(const LALR_state* state,
                                     int  reduce_counts[],
                                     int  max_count) const
{
  memset(reduce_counts, 0, grammar->n_prods() * sizeof(int));
  int  max_idx = -1;
  for (LALR_action::set::const_iterator iact  = state->actions.begin();
                                        iact != state->actions.end();
                                      ++iact) {
    const LALR_action* act = *iact;
    if (act->getf(LALR_action::DO_REDUCE)) {
      ASSERT(act->la_term->is_terminal());
      int idx = act->reduce_prod->id;
      if (max_count < ++reduce_counts[idx]) {
        max_count = reduce_counts[idx];
        max_idx   = idx;
      }
    }
  }
  return max_idx;
}

LALR_table::LALR_table()
{
  grammar = 0;
  states.owns_data(true);
  id_to_state.owns_data(false);
  conflicts.owns_data(false);
  msg_listener = 0;
}

// Constructs this LALR_table based on the given grammar.
//
void  LALR_table::prepare_from(Grammar& grammar)
{
  this->grammar = &grammar;
  states.flush();
  id_to_state.flush();
  conflicts.flush();

  LALR_state* start_state = new LALR_state(0);
  Term* ini_term = grammar.term(Grammar::INI_TERM);
  // generate the core item set of the start state:
  for (Term::prods_iterator pi = ini_term->prods_begin(), end = ini_term->prods_end();
                            pi != end; ++pi) {
    LALR_item* item = new LALR_item(*pi, 0);
    start_state->items.insert(item);
  }
  start_state->n_core_items = start_state->items.size();
  id_to_state.push(start_state);
  states.insert(start_state);

  if (msg_listener != 0)
    msg_listener->message("preparing lalr table");
  // generate states:
  for (int i = 0; i < n_states(); ++i) {
    LALR_state* state = id_to_state[i];
    make_closure(state);
    calc_transitions(state);
  }
  if (msg_listener != 0)
    msg_listener->messagef("%d states calculated", n_states());

  calc_la_terms();
  if (msg_listener != 0)
    msg_listener->message("look-aheads propagated");

  calc_reductions();
  if (msg_listener != 0)
    msg_listener->message("reductions calculated");
}

void  LALR_action::add_reduce(Prod* prod)
{
  ASSERT(getf(DO_ACCEPT) == 0);
  if (reduce_prod != 0) { // oops ! a R-R conflict.
    if (rest_reduce_prods == 0) {
      // so far it is a single R-R conflict
      flags &= ~IS_FLAGS;
      flags |=  IS_CONFLICT;
      rest_reduce_prods = new Prod::set(!OWNS_DATA);
    }
    // the rule with smaller id takes precedence:
    if (reduce_prod->id < prod->id) {
      rest_reduce_prods->insert(prod);
    }
    else {
      rest_reduce_prods->insert(reduce_prod);
      reduce_prod = prod;
    }
  }
  else { // no R-R conflict(s) (yet)
    if (shift_state != 0) {// oops ! a S-R conflict.
      flags &= ~IS_FLAGS;
      flags |=  IS_CONFLICT;
    }
    else {
      flags |=  DO_REDUCE;
    }
    reduce_prod = prod;
  }
}

// Implements the classical method for resolving a shift-reduce
// conflict on a precedence-associativity basis.
//
bool   LALR_action::resolve_conflicts()
{
  // clear the flags (but only those known to us: IS_XXX & DO_XXX):
  flags &= ~(IS_FLAGS | DO_FLAGS);

  if (0 != rest_reduce_prods) {
    // reduce-reduce conflict(s) !
    flags |= IS_CONFLICT;
    if (shift_state == 0) {
      // it is a RR conflict only !
      flags |= DO_REDUCE;
      return false;
    }
  }

  ASSERT(shift_state != 0 && reduce_prod != 0);

  if (reduce_prod->prio.is_null()) {
    Term* prio_term = reduce_prod->get_rightmost_prio_term();
    if (prio_term == 0) {
      flags |= IS_CONFLICT | DO_SHIFT;
      return false;
    }
    reduce_prod->prio = prio_term->prio;
  }
  if (la_term->prio.is_null()) {
    flags |= IS_CONFLICT | DO_REDUCE;
    return false;
  }

  Prio s_prio = la_term->prio;      ASSERT(!s_prio.is_napp());
  Prio r_prio = reduce_prod->prio;  ASSERT(!r_prio.is_napp());

  // in case there was no reduce-reduce conflict and we've reached that far,
  // the conflict is surely resolvable
  if (0 == (flags & IS_CONFLICT)) {
    flags |= IS_RESOLVED;
  }

  // both priorities are defined; first compare prec(s):
  if (r_prio.prec() < s_prio.prec()) { // resolve as shift:
    flags |= DO_SHIFT;
    return true;
  }
  if (r_prio.prec() > s_prio.prec()) { // resolve as reduce:
    flags |= DO_REDUCE;
    return true;
  }

  // on equal prec consider the assoc;
  // everyone who relies on this conflict resolution method,
  // should enforce that s_prio.prec() == r_prio_prec() results in:
  ASSERT(s_prio.assoc() == r_prio.assoc());

  switch (s_prio.assoc()) {
    case ASSOC_LEFT:  //resolve as reduce
      flags |= DO_REDUCE;
      break;
    case ASSOC_RIGHT: //resolve as shift
      flags |= DO_SHIFT;
      break;
    case ASSOC_NONE:  //reslove as error
      flags |= DO_ERROR;
      break;
    default:
      ASSERT(0);
  }
  return true;
}


int  LALR_state::set_traits::compare(DataKey core_p,
                                     DataKey core_q)
{
  if (core_p.n_core_items != core_q.n_core_items) {
    return core_p.n_core_items - core_q.n_core_items;
  }
  LR0_item::set::const_iterator p = core_p.items.begin();
  LR0_item::set::const_iterator q = core_q.items.begin();
  int n = core_p.n_core_items;

  while (n-- != 0) {
    int diff = LALR_item::set_traits::compare(*p, *q);
    if (diff) {
      return diff;
    }
    ++p;
    ++q;
  }
  return 0;
}
