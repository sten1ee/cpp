#ifndef jacc_h
#define jacc_h

#include "lalr.h"
#include "jacc_glue.h" // for enum tnt_t
#include "jacc_jamp.h"
#include "jacc_symtype.h"

struct jacc_symtype;
struct jacc_sval;
struct jacc_sval_env;
struct jacc_term;
struct jacc_prod;
struct jacc_grammar;


struct jacc_sval
{
    int           idx;     // index in the rhs
    jacc_symtype* symtype; // type of the value
    const char*   alias;

    jacc_sval(int i, jacc_symtype* st, const char* a)
      : idx(i), symtype(st), alias(a)
      {;}

    struct traits
    {
      typedef       jacc_sval   Data;
      typedef const jacc_sval&  DataIn;
      typedef const jacc_sval&  DataOut;
    };

    typedef vector< vector_traits< jacc_sval > > vector;
};


struct jacc_sval_env : private jacc_sval::vector
{
  typedef jacc_sval::vector       parent;
  typedef parent::const_iterator  const_iterator;

  const int   top_idx;

  jacc_sval_env(const jacc_sval_env& v)
    : parent(v),
      top_idx(v.top_idx)
    {;}

  jacc_sval_env(const parent& v, int i)
    : parent(v),
      top_idx(i)
    {;}

  using parent::begin;
  using parent::end;
  using parent::size;
  using parent::operator[];
};


struct jacc_term : Term
{
  tnt_t         tnt;
  jacc_symtype* symtype;

  jacc_term(int id, const string& name)
    : Term(id, name),
      tnt(TNT_UNDEF), symtype(0)
    {;}
};


struct jacc_prod : Prod
{
  jacc_sval_env  sval_env;
  const char*    action_code;
  jacc_term*     prec_term;
  const char*    when_code;
  const char*    with_code;

  jacc_prod(Term*                lhs_term,
            Term::array&         rhs_terms,
            const jacc_sval_env& sval_env)
    : Prod(lhs_term, rhs_terms),
      sval_env(sval_env)
    {;}

  // renders some human-readable description of the production
  // (e.g. 'exp ::= exp(a) PLUS exp(b)')
  void  render(std::ostream& out) const;
  // checks if this is a phantom prod created to execute a midrule action:
  bool  is_mra_prod() const { return lhs_term->name[0] == '@'; }
};


struct jacc_grammar : Grammar
{
  struct jacc_term_factory : TermFactory
  {
    public:
      virtual Term* create_term(int id, const string& name)
        {
          return new jacc_term(id, name);
        }
  };

protected: jacc_term_factory* tf;
  // The user specified start term (not the ini term S' !)
protected: jacc_term*         start_term;

public:
  jacc_grammar()
    : Grammar(tf = new jacc_term_factory),
      start_term(0)
    {;}

 ~jacc_grammar()
    {
      delete tf;
    }

  jacc_term* term(int i)
    {
      return SURE_CAST(Grammar::term(i), jacc_term);
    }

  jacc_term* term_by_name(const string& name)
    {
      return SURE_CAST(Grammar::term_by_name(name), jacc_term);
    }

  void  add_start_term(Term* t)
    {
      ASSERT(0); // use set_start_term() instead !
    }

  jacc_term*  get_start_term() const
    {
      return start_term;
    }

  void        set_start_term(jacc_term* jt)
    {
      ASSERT(start_term == 0);
      Grammar::add_start_term(jt);
      start_term = jt;
    }
};

struct jacc_table : LALR_table {};

struct jacc : jacc_jamp
{
    int  process(int argc, const char* argv[]);

  protected:
    jacc_grammar      grammar;
    jacc_table        table;
    jacc_symtype_mgr  symtype_mgr;

    // A flag indicating whether the action table should be compressed
    // by using default reduce entries:
    //
    bool              compress_table;

    // The parse method resides in jacc_glue.cpp
    void  parse_grammar(std::istream& grm_file, const char* grm_file_name);

    void  render_action_table(std::ostream& out);
    void  render_reduce_table(std::ostream& out);
    void  render_production_table(std::ostream& out);
    void  render_do_action(std::ostream& out);
    void  render_delete_table(std::ostream& out);
    void  render_symtype_enum(std::ostream& out);
    void  render_symtype_defs(std::ostream& out);

    // method inherited from class jacc_jamp:
    virtual bool  try_handle_directive(const string& directive_name);

    static bool  code_refers_to_ident(const char* code, const char* ident);
    static int   count_terminal_actions(const LALR_action::set& actions);
    static int   count_nonterminal_actions(const LALR_action::set& actions);
};

#endif //jacc_h
