#ifndef PG_LR1_h
#define PG_LR1_h

#include "pg/grammar.h"
#include "util/progress.h"

class LR0_item;
class LR1_spot;
class LR1_state;
class LR1_action;
class LR1_table;


class LR0_item
{
  public:
    Prod*     prod;
    int       rhsPos;  //dot position in range [0, prod->term.size()]

    virtual   ~LR0_item() {}

    LR0_item(Prod* _prod, int _rhsPos)
      : prod(_prod), rhsPos(_rhsPos) {}

    class Traits : public VectorPtrTraits< LR0_item >
    {
      public:
        static int  Compare(const LR0_item* i, const LR0_item* j);
    };

    bool  isPast() const { return  rhsPos == prod->rhsTerms.size(); }
    Term* topicalTerm() const { return  prod->rhsTerms[rhsPos]; }

    typedef SortedVector< Traits >                  Set;
};

// LR0_items wich differ only on the laTerm are joined in spot:
class LR1_spot : public LR0_item
{
  public:
    Term::Set   laTerms; //lookahead symbols
    bool        lr_mode; //should Traits::Compare consider spot's laTerms ?

    LR1_spot(Prod* _prod, int _rhsPos, Term::Set& _laTerms)
      : LR0_item(_prod, _rhsPos),
        laTerms(_laTerms), lr_mode(true) {}

    class Traits : public VectorPtrTraits< LR1_spot >
    {
      public:
        static int  Compare(const LR1_spot* i, const LR1_spot* j);
    };

    typedef   Vector< VectorPtrTraits< LR1_spot > >   Vector;
    typedef   SortedVector< Traits >  Set;
};


class LR1_action
{
  public:
    Term*             laTerm;
    LR1_state*        shiftState;
    LR0_item::Set     shiftItems;  //used to determine shiftState
    Prod::Set         reduceProds;
    bool              accept;

    LR1_action(Term* term)
      : laTerm(term),
        shiftState(0),
        shiftItems(4, DOUBLE_INCREMENT),
        reduceProds(1, DOUBLE_INCREMENT),
        accept(false)
            { reduceProds.ownsData(false); }
    void  addReduce(Prod* prod)
            { reduceProds.insertIfNotExists(prod); }

    bool  has_conflict() const;
    int   resolve_conflict();

    class Traits : public VectorPtrTraits< LR1_action >
    {
      public:
        typedef Term* DataId;
        static int Compare(const Term* t, const LR1_action* a)
          { return (t == a->laTerm) ? 0
                                    : (t->id < a->laTerm->id) ? -1
                                                              : +1;
          }
        static int Compare(const LR1_action* a, LR1_action* b)
          { return Compare(a->laTerm, b); }
    };

    typedef SortedVector< Traits >  Set;
};


class LR1_state
{
  public:
    int               id;
    LR1_spot::Set     spots;
    LR1_action::Set   actions;
    LR1_state*        redu_state; // (opt) the LALR-reduction of this state

    LR1_state(int _id, LR1_spot::Set& _spots)
      : id(_id), spots(_spots), redu_state(0) {}

    LR1_action* action_on(Term* term);

    class Traits : public VectorPtrTraits< LR1_state >
    {
      public:
        typedef LR1_spot::Set*  DataId;
        static int  Compare(const LR1_spot::Set* spots, const LR1_state* state);
        static int  Compare(const LR1_state* s1, const LR1_state* s2)
          { return  Compare(&s1->spots, s2); }
    };

    typedef Vector< VectorPtrTraits< LR1_state > >  Vector;
    typedef SortedVector< Traits >                  Set;
};


class LR1_conflict
{
  public:
    LR1_state*  state;
    LR1_action* action;
    int         type;   //some bit or-ed combination of SR and RR

    enum  { SR = 1, //Shift-Reduce
            AR = 2, //Accept-Reduce
            RR = 4  //Reduce-Reduce
          };

    LR1_conflict(LR1_state* _state, LR1_action* _action, int _type)
      : state(_state), action(_action), type(_type) {}

    typedef Vector< VectorPtrTraits< LR1_conflict > >  Vector;
};


class LR1_table
{
  public:
    Grammar&              grammar;

  protected:
    LR1_state::Set        states;
    LR1_state::Vector     id_to_state;
    LR1_conflict::Vector  conflicts;

    LR1_table();

    static LR1_spot::Set  items_closure(LR0_item::Set& items);
    LR1_state*    make_state(LR1_spot::Set& spotSet);
    void          calc_actions(LR1_state* state);
    void          calc_states(ProgressBar& pb);
    void          resolve_conflicts();

  public:
    LR1_table(Grammar& grammar, ProgressBar& pb);

    int           nStates()    const { return states.size();    }
    LR1_state*    state(int i)       { return id_to_state[i];   }
    LR1_state*    startState()       { return id_to_state[0];   }
    int           nConflicts() const { return conflicts.size(); }
    LR1_conflict* conflict(int i)    { return conflicts[i];     }
};

#endif//PG_LR1_h
