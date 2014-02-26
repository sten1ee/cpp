#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "util/set.h"
#include "util/vector.h"
#include "util/string.h"
#include "util/array.h"
#include "util/alloc.h"

#include "prio.h"

class Term;
class Prod;
class TermFactory;
class Grammar;

class Term
{
  public:
    int           id;
    string        name;
    bool          nullable;
    Prio          prio;

  protected:
    friend class Grammar;
    array< Prod* >   _prods;

  public:
    typedef array< Prod* >::iterator         prods_iterator;
    typedef array< Prod* >::const_iterator   prods_const_iterator;

    prods_iterator        prods_begin()       { return _prods.begin(); }
    prods_iterator        prods_end  ()       { return _prods.end();   }
    prods_const_iterator  prods_begin() const { return _prods.begin(); }
    prods_const_iterator  prods_end  () const { return _prods.end();   }
    int                   n_prods    () const { return _prods.size();  }

    bool  operator == (const Term& t) const { return this == &t; }
    bool  operator <  (const Term& t) const { return id < t.id;  }

    typedef vector< vector_traits_ptr< Term > >  vector;

    struct set_traits : set_traits_ptr< Term >
    {
      static int compare(const Term* i, const Term* j)
        { return i->id - j->id; }
    };
    typedef set< set_traits >  set;
    typedef array< Term* >     array;

    Term::set     first_set;

    virtual  ~Term() {}
              Term(int _id, const string& _name);

    bool  is_terminal() const { return _prods.empty(); }

    FAST_ALLOC
};


class Prod
{
  public:
    int           id;
    Term*         lhs_term;
    Term::array   rhs_terms;
    Prio          prio;

    bool  operator == (const Prod& p) const { return this == &p; }
    bool  operator <  (const Prod& p) const { return id < p.id;  }

    // Returns the rightmost rhs term that has prio defined or 0 if none has.
    Term* get_rightmost_prio_term() const;

    class ProdTraits : public set_traits_ptr< Prod >
    {
      public: static int compare(const Prod* i, const Prod* j)
        { return i->id - j->id; }
    };
    typedef  vector< vector_traits_ptr< Prod > > vector;
    typedef  set< ProdTraits >  set;
    typedef  array< Prod* >     array;

    virtual  ~Prod() {}
              Prod(Term* lhs_term, Term::array& rhs_terms);

    FAST_ALLOC
};


class TermFactory
{
  public:
    virtual Term* create_term(int term_id, const string& term_name)=0;
};

class Grammar
{
  private:
    Term::vector  terms;
    Prod::vector  prods;

    // The term factory provides a machanism to use custom Term objects
    // (i.e. instances of a user supplied subclass of Term).
    // Note that there is no need of prod factory, as Prod objects are
    // created by the user himself in constrast to the Term objects which are
    // created in the term_by_name method.
    TermFactory*  term_factory;

    int   _max_rhs_size;

    void  calc_nullables();
    void  calc_first_sets();

  public:
             Grammar(TermFactory* tf=0);
    virtual ~Grammar() {;}

    enum  { INI_TERM, ERR_TERM, EOF_TERM };

    int   n_terms       ()      const       { return terms.size(); }
    Term* term          (int i)             { return terms[i];     }
    Term* term_by_name  (const string& name);
    int   n_prods       ()      const       { return prods.size(); }
    Prod* prod          (int i)             { return prods[i];     }
    int   max_rhs_size  ()      const       { return _max_rhs_size;}
    void  add_start_term(Term* start_term);
    void  add_prod(Prod* prod);
    void  prepare();
};

#endif//GRAMMAR_H
