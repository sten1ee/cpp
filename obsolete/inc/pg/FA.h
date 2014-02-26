#ifndef PG_FA_H
#define PG_FA_H

#include "util/vector.h"
#include "pg/predefs.h"


class Transition
{
  public:
    Char    charr; //transition char
    State*  state; //transition state

                 Transition() : charr(0), state(0) {}
                 Transition(Char charr, State* state)
                              : charr(charr), state(state) {}

    typedef vector< vector_traits< Transition > >  vector;
};


class State
{
  public:
    int                 id;
    int                 code;
    Transition::vector  transitions;
    Transition::vector  eps_transitions;

    enum { FINAL_MASK = INT_MAX, START_FLAG = INT_MIN };

            int  finalCode() const { return code & FINAL_MASK; }
            int  isStart()   const { return code & START_FLAG; }

                 //State(int id_=0, unsigned code_=0) : id(id_), code(code_) {}
                 State() : id(-1), code(0) {}
    virtual     ~State() {}
    virtual int  compareTo(const State* s) const;

    typedef vector< vector_traits_ptr< State > >  vector;
};


class FA
{
    char*          _name;  //machine name
    State::vector  states; //machine state list

  public:
              FA(const char* name)
              : _name(strcpy(new char[1 + strlen(name)], name)) {}
              FA(char* name, bool ownsData=false)
              : _name(ownsData ? name
                               : strcpy(new char[1 + strlen(name)], name)) {}
    virtual  ~FA() { delete[] _name; }

    State*      newState();
    State*      regState(State* s);
    void        transition(State* beg, Char charr, State* end);
    void        swapStates(int s1, int s2);

    int         nStates() const { return states.size(); }
    State*      state(int si)   { return states[si]; }
    const char* name()    const { return _name; }
};

#endif//PG_FA_H
