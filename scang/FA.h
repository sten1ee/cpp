#ifndef PG_FA_H
#define PG_FA_H

#include "predefs.h"


class FinalCode
{
    int val;

  public:
    // final code policy:
    enum policy_t { SINGLE = 1, MULTIPLE = 2 };

    explicit FinalCode(int v) : val(v) {;}

    int value() const { return val; }

    struct sv_traits : sortvec_traits<FinalCode>
      {
        static int  compare(FinalCode f1, FinalCode f2)
          { return f1.value() < f2.value() ? -1
                 : f1.value() > f2.value() ? +1 : 0; }
      };

    typedef sorted_vector< sv_traits > set;
};


class Transition
{
  public:
    Char    charr; //transition char
    State*  state; //transition state

                 Transition() : charr(0), state(0) {}
                 Transition(Char charr, State* state)
                              : charr(charr), state(state) {}

    struct sv_traits : sortvec_traits< Transition >
      {
        typedef Char    DataKey;
        static  DataKey keyof(const Transition& t)
          { return t.charr; }
        static  int     compare(DataKey k1, DataKey k2)
          { return CharSet::Traits::compare(k1, k2); }
        static  bool    allow_duplicates()
          { return true; }
      };

    typedef sorted_vector< sv_traits > set;
};


class State
{
  public:
    typedef Transition::set               Transitions;
    typedef Transitions::const_iterator   TransitionsIter;

    typedef FinalCode::set                FinalCodes;
    typedef FinalCodes::const_iterator    FinalCodesIter;

  private:
    friend class FA;
    friend class MultiState;

    int           _id;
    Transitions   _transitions;
    FinalCodes    _final_codes;
    bool          _is_start;

    State() : _id(-1), _final_codes(0, 2), _is_start(false) {}


  public:
    virtual     ~State() {;}
    virtual int  compareTo(const State* s)  const { return _id - s->_id; }

          int             id()              const { return _id; }
          bool            is_start()        const { return _is_start; }
          void            set_is_start(bool s)    { _is_start = s; }
    const FinalCodes&     final_codes()     const { return _final_codes; }
          void            set_final_code(FinalCode fc);
    const Transitions&    transitions()     const { return _transitions; }
    const State*          trans_state(Char charr) const;
          State*          trans_state(Char charr);

    struct sv_traits : sortvec_traits_ptr< State >
      {
        static int  compare(const State* s1, const State* s2)
          { return s1->compareTo(s2); }
      };

    typedef vector< vector_traits_ptr< State > >  vector;
    typedef sorted_vector< sv_traits >            set;
};


class FA
{
  public:
    typedef State::vector          States;
    typedef States::const_iterator StatesIter;

  private:
    const char*   _name;   //machine name
          States  _states; //machine state list

    State*  state(int si) { return _states[si]; }
    State*  reg_state(State* s);
    void    swap_states(int s1, int s2);

  public:
              FA(const char* name, bool ownsData=false);
    virtual  ~FA() { delete[] _name; }

          State*    new_state();
          void      transition(State* beg, Char charr, State* end);


    const char*     name()       const { return _name; }
    const States&   states()     const { return _states; }
    const State*    state(int i) const { return _states[i]; }
          int       n_states()   const { return _states.size(); }

    static FA*  NFA_to_MDFA (FA& nfa, FinalCode::policy_t fcp);
    static FA*  MDFA_to_DFA (FA& mdfa);
    static FA*  DFA_to_OMDFA(FA& dfa);
    static void DFA_to_CDFA(FA& dfa);

    static void NFA_join_RE (FA& nfa, RE* re, FinalCode fc);
    // converts a NFA to a Optimal DFA:
    static FA*  NFA_to_ODFA(FA& nfa, FinalCode::policy_t fcp);
};

// inlines:
inline
void  State::set_final_code(FinalCode fc)
  {
    ASSERT(_final_codes.empty());
    _final_codes.insert_back(fc);
  }

inline
const State* State::trans_state(Char charr) const
  {
    return const_cast<State*>(this)->trans_state(charr);
  }

inline
State* State::trans_state(Char charr)
  {
    Transition::set::finger f = _transitions.find(charr);
    return (f ? (*f).state : 0);
  }

inline
FA::FA(const char* name, bool ownsData)
  {
    _name = ownsData ? name : strcpy(new char[1 + strlen(name)], name);
  }

#endif//PG_FA_H
