#ifndef PG_FAINTERN_H
#define PG_FAINTERN_H

#include "fa.h"

// (internal) class GotoTransition used in NFA to DFA conversion
//----------------------------------------------------------
class GotoTransition
{
  public:
    Char        charr;
    State::set  states;

    GotoTransition() : charr(0), states(!OWNS_DATA) {}
    GotoTransition(Char c) : charr(c), states(!OWNS_DATA) {}

    struct sv_traits : sortvec_traits_ptr< GotoTransition >
      {
        typedef Char    DataKey;
        static  DataKey keyof(const DataIn gt) { return gt->charr; }
        static  int     compare(DataKey k1, DataKey k2) { return k1 - k2; }
      };

    typedef sorted_vector< sv_traits >  set;
};


// (internal) class MultiState used in NFA to DFA conversion
//----------------------------------------------------------
class MultiState : public State//, public
{
  public:
    const State::set  states;

    MultiState(State::set& ss)
      : states(ss)
      {;}

    virtual int                compareTo(const State* s) const;

            void                genFinalCodes(FinalCode::policy_t fcp);
    static  void                Closure(State::set& states);
    static  GotoTransition::set Goto(const State::set& states);
    static  CharSet             GetTransChars(const State::set& states);
};

#endif//PG_FAINTERN_H
