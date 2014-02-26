#ifndef PG_FAINTERN_H
#define PG_FAINTERN_H

#include "util/sortvec.h"

// Stuff needed to make Set of State(s):
//----------------------------------------------------------
struct StateSVTraits : sortvec_traits_ptr< State >
{
  static int  compare(const State* s1, const State* s2)
    { return s1->compareTo(s2); }
};

typedef sorted_vector< StateSVTraits >  StateSet;

// (internal) class MultiState used in NFA to DFA conversion
//----------------------------------------------------------
class MultiState : public State, public StateSet
{
  public:
    MultiState(StateSet& ss) : StateSet(ss) { owns_data(false); }
    MultiState() { owns_data(false); }

    virtual int  compareTo(const State* s) const;
};

// (internal) class GotoTransition used in NFA to DFA conversion
//----------------------------------------------------------
class GotoTransition
{
  public:
    Char      charr;
    StateSet  states;

    GotoTransition() : charr(0), states(!OWNS_DATA) {}
    GotoTransition(Char c) : charr(c), states(!OWNS_DATA) {}
};

// Stuff needed to make Set of GotoTransition(s):
//----------------------------------------------------------
struct GotoTransitionSVTraits : sortvec_traits_ptr< GotoTransition >
{
  static int  compare(const GotoTransition* gt1, const GotoTransition* gt2)
    {
      return  gt1->charr == gt2->charr ? 0
                                       : gt1->charr < gt2->charr ? -1
                                                                 : +1;
    }
};

typedef sorted_vector< GotoTransitionSVTraits >  GotoTransitionSet;

// Stuff needed to make Set of Char(s):
//----------------------------------------------------------
struct CharSVTraits : sortvec_traits< Char >
{
  static int  compare(Char c1, Char c2)
    { return (int)c1 - c2; }
};


typedef sorted_vector< CharSVTraits >  CharSet;

#endif//PG_FAINTERN_H
