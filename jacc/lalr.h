#ifndef LALR_H
#define LALR_H

#include "grammar.h"


class LR0_item;
class LALR_stub;
class LALR_item;
class LALR_state;
class LALR_action;
class LALR_table;
class msg_listener; // abstract class located in util/msg_listener.h

class LR0_item
{
  public:
    Prod*   prod;
    int     rhs_pos;     // dot position in range [0, prod->rhs_terms.size()]

    virtual ~LR0_item() {;}

    LR0_item(Prod* _prod, int _rhs_pos)
      : prod(_prod), rhs_pos(_rhs_pos) {;}

    struct  set_traits : set_traits_ptr< LR0_item >
    {
      static  int compare(const LR0_item* i, const LR0_item* j)
      {
        // Note that each core-item < each non-core-item:
        if (i->rhs_pos != j->rhs_pos)
          return (j->rhs_pos - i->rhs_pos);
        else
          return (i->prod->id - j->prod->id);
      }
    };

    typedef ::vector< vector_traits_ptr< LR0_item > > vector;
    typedef ::set< set_traits > set;

    FAST_ALLOC
};


class LALR_item : public LR0_item
{
  public:
    Term::set       la_terms; // the set of already propagated la terms;
    Term::set   new_la_terms; // the set of (yet) not propagated la terms;

    LALR_item::vector  items_to_notify; // the propagate-to set of items;


    virtual   ~LALR_item() {}

    LALR_item(Prod* prod, int rhs_pos)
      : LR0_item(prod, rhs_pos),
        la_terms(!OWNS_DATA), new_la_terms(!OWNS_DATA),
        items_to_notify(!OWNS_DATA)
      {;}

    bool  is_past()      const { return  rhs_pos == prod->rhs_terms.size(); }
    Term* topical_term() const { return  prod->rhs_terms[rhs_pos]; }
};


class LALR_stub : public LR0_item
{
  public:
    LALR_item*  item_from;
    LALR_item*  item_to;

    LALR_stub(Prod* prod, int rhs_pos) : LR0_item(prod, rhs_pos) {;}
};


class LALR_action
{
  public:
    // The lookahead symbol that this action is taken on:
    Term*             la_term;

    // The state this action belongs to
    // (only conflicting actions have this property set !) :
    LALR_state*       host_state;

    // The state to goto after the lookahead is shifted:
    LALR_state*       shift_state;

    // Used only in method 'LALR_table::calc_transitions()'
    // to determine the shift_state:
    LR0_item::set*    shift_stubs;

    // The production to reduce by (may be null) :
    Prod*             reduce_prod;

    // If the state has R-R conflict(s) on that action's lookahead,
    // that's where the rest competing reduce-by productions are held
    // (the best choise is kept in reduce_prod) :
    Prod::set*        rest_reduce_prods;

    int   flags;
    void  setf  (int f)       { flags |=  f; }
    void  unsetf(int f)       { flags &= ~f; }
    int   getf  (int f) const { return flags & f; }

    enum
    {
      // The IS_XXX flags are mutually exclusive flags that designate
      // "the conflict state of this action" :
      // IS_NORMAL   - action originally having no conflict(s)
      //               (i.e. single shift or reduce thus nothing to resolve);
      // IS_RESOLVED - action having a single SR conflict resolved by p&a
      //               (in bison a SR conflict can be resolved to
      //               a shift, reduce, or error (see %nonassoc));
      // IS_CONFLICT - action having SR and/or RR conflict(s)
      //               that could not be resolved by p&a
      //               (and thus is resolved by default);
      IS_NORMAL   = 0x01,
      IS_RESOLVED = 0x02,
      IS_CONFLICT = 0x04,
      IS_FLAGS    = 0xFF, // mask for the IS_xxx flags;


      // The DO_XXX flags are mutually exclusive flags that designate
      // "the action of this action" :
      DO_SHIFT    = 0x0100,
      DO_REDUCE   = 0x0200,
      DO_ACCEPT   = 0x0400,
      DO_ERROR    = 0x0800,
      DO_FLAGS    = 0xFF00, // mask for the DO_xxx flags;

      // The flags assigned to an action upon its construction :
      INITIAL_FLAGS = IS_NORMAL
    };

    LALR_action(Term* term)
      : la_term(term),
        host_state(0),
        shift_state(0),
        shift_stubs(0),
        reduce_prod(0),
        rest_reduce_prods(0),
        flags(INITIAL_FLAGS)
      {;}

   ~LALR_action() //(will be virtual in future)
      { if (rest_reduce_prods != 0) delete rest_reduce_prods; }

    void  add_stub(LALR_stub* stub)
      {
        if (shift_stubs == 0) {
          shift_stubs = new LR0_item::set(OWNS_DATA);
        }
        shift_stubs->insert_back(stub);
      }

    bool  has_stubs() const { return shift_stubs != 0; }

    LR0_item::set& get_stubs() { return *shift_stubs; }

    void  dispose_of_stubs()
      {
        if (shift_stubs != 0) {
          delete shift_stubs;
          shift_stubs = 0;
        }
      }

    void  add_reduce(Prod* prod);

    // This method should be invoked only when this action has conflicts.
    // It returns true iff the conflict(s) was/were resolved.
    // May clear the IS_CONFLICT bit and set the IS_RESOLVED
    // and some of the DO_XXX bits.
    //
    bool  resolve_conflicts(); //(will be virtual in future)

    struct set_traits : set_traits_ptr< LALR_action >
    {
        typedef const Term* DataKey;

        static DataKey  keyof(const LALR_action* a)
          { return a->la_term; }
        static int compare(const Term* t1, const Term* t2)
          { return Term::set_traits::compare(t1, t2); }
    };

    typedef ::vector< vector_traits_ptr< LALR_action > > vector;
    typedef ::set< set_traits >  set;

    FAST_ALLOC
};



class LALR_state
{
  public:
    int               id;
    LALR_item::set    items;        // all LALR items in this state
    int               n_core_items; // # of core LALR items
    LALR_action::set  actions;      //(note that they are the 1st n in the set)


    LALR_state(int _id)
      : id(_id) {;}

    LALR_action* action_on(Term* term);

    struct core
    {
      core(const LR0_item::set& _items, int _n_core_items)
        : items(_items), n_core_items(_n_core_items)
        {;}

      const LR0_item::set& items;
      const int            n_core_items;
    };

    struct set_traits : set_traits_ptr< LALR_state >
    {
      public:
        typedef const core  DataKey;

        static  DataKey keyof(DataIn lalr_state)
          { return core(lalr_state->items, lalr_state->n_core_items); }
        static  int     compare(DataKey p, DataKey q);
    };

    typedef ::vector< vector_traits_ptr< LALR_state > >  vector;
    typedef ::set< set_traits >  set;

    FAST_ALLOC
};



class LALR_table
{
  public:
    Grammar*               grammar;

  protected:
    LALR_state::set        states;
    LALR_state::vector     id_to_state;
    LALR_action::vector    conflicts;

    void          make_closure(LALR_state* state);
    void          calc_transitions(LALR_state* state);
    LALR_state*   stubs_to_state(LALR_stub::set& stubs);
    void          calc_la_terms();
    void          calc_reductions();

  public:
    // A message listener (if installed) receives a human readable message
    // when a specific stage of the table preparation process is finished:
    //
    class msg_listener* msg_listener;

    LALR_table();

    int           n_states()      const { return states.size();    }
    LALR_state*   state(int i)          { return id_to_state[i];   }
    LALR_state*   start_state()         { return id_to_state[0];   }
    int           n_conflicts()   const { return conflicts.size(); }
    LALR_action*  conflict(int i)       { return conflicts[i];     }

    // Returns the number of conflicts that failed to resolve:
    int           resolve_conflicts();

    // Returns the id of the most frequently reduce-by rule for the given state.
    // Only rules used more than max_count times are considered.
    // As a side-effect the reduce_counts array (of size >= grammar.n_prods())
    // holds the reduce frequencies of the grammar productions for this state.
    //
    int           calc_default_reduce(const LALR_state* state,
                                      int  reduce_counts[],
                                      int  max_count=1) const;

    // Constructs this LALR_table based on the given grammar.
    void          prepare_from(Grammar& grammar);
};

#endif//LALR_H
