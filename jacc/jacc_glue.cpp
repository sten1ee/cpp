#include <stdio.h>
#include <stdarg.h>

#include "jacc_glue.h"
#include "jacc.h"

// depending on its tag value rhs_part is either a symbol or an action.
//
struct rhs_part {
  enum tag_t { SYMBOL, ACTION };
  tag_t       tag;
    union {
  const char* symbol_name;
  const char* action_code;
    };
  const char* alias;   // alias for the semantic value of the symbol / action

  typedef vector< vector_traits< rhs_part > >  vector;

  rhs_part(const rhs_part& p)
    {
      tag = p.tag;
      symbol_name  = /*action_code =*/ p.symbol_name;
      alias = p.alias;
    }

  rhs_part(tag_t t, const char* s_a, const char* a)
    {
      tag   = t;
      symbol_name  = /*action_code =*/ s_a;
      alias = a;
    }
};

// holds the information of a single rule's right-hand-side.
//
struct rhs {
  jacc_term*        prec_term;
  const char*       when_code;
  const char*       with_code;

  rhs_part::vector  parts;

  rhs()
    {
      prec_term = 0;
      when_code = with_code = 0;
    }

  void reset()
    {
      prec_term = 0;
      when_code = with_code = 0;
      parts.flush();
    }
};

// all global data used by the jg_xxx() functions
// is contained in the 'current' struct variable.
//
static struct current {
  jacc_lexer*         lexer;      // ptr to extern data (see parse_grammar)
  jacc_symtype_mgr*   symtype_mgr;// ptr to extern data (see parse_grammar)
  jacc_grammar*       grammar;    // ptr to extern data (see parse_grammar)
  jacc_property_set*  properties; // ptr to extern data (see parse_grammar)

  tnt_t             tnt;
  jacc_symtype*     symtype;
  int               prec;
  assoc_t           assoc;
  jacc_term*        lhs_term;
  struct rhs        rhs;
  int               n_mra; // number of mid-rule actions generated so far
} current;

// interface to bison:
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void  jacc::parse_grammar(std::istream& grm_file, const char* grm_file_name)
{
  current.lexer       =  this;  // jacc subclasses jamp subclasses lexer !
  current.grammar     = &grammar;
  current.properties  = &macros;
 (current.symtype_mgr = &symtype_mgr)->set_log(get_log());

  current.rhs.reset();
  current.n_mra = 0;
  current.prec  = 0;

  jacc_source  grm_source(grm_file, grm_file_name);
  push_source(&grm_source);
  yyparse();
  pop_source();
}

void  jg_prop(const char* prop_name, const char* prop_value)
{
  if (current.properties->defined(prop_name)) {
    jg_errorf("Redefinition of property `%s'", prop_name);
  }
  current.properties->property(prop_name).set_val(prop_value);
}

void  jg_type(enum tnt_t tnt, const char* type_name, const char* value_type)
{
  current.tnt = tnt;
  current.symtype = &current.symtype_mgr->symtype_for(type_name, value_type);
}

void  jg_type_term(const char* term_name)
{
  jacc_term* term = current.grammar->term_by_name(term_name);
  if (term->tnt != TNT_UNDEF) {
    jg_errorf("Type redefinition for `%s' (skipping)", term->name.c_str());
    return;
  }
  term->tnt = current.tnt;
  term->symtype = &current.symtype_mgr->
                      specz_symtype_for(*current.symtype, term);
}

void  jg_prec(enum prec_t prec)
{
  current.prec++;
  switch (prec) {
    case PREC_LEFT    : current.assoc = ASSOC_LEFT ; break;
    case PREC_RIGHT   : current.assoc = ASSOC_RIGHT; break;
    case PREC_NONASSOC: current.assoc = ASSOC_NONE ; break;
    default: ASSERT(0);
  }
}

void  jg_prec_term(const char* term_name)
{
  jacc_term* term = current.grammar->term_by_name(term_name);
  if (!term->prio.is_null()) {
    jg_errorf("Precedence redefinition for `%s' (skipping)",
              term->name.c_str());
    return;
  }
  if (term->tnt == TNT_NONTERMINAL) {
    jg_errorf("Precedence definition for nonterminal `%s' (skipping)",
              term->name.c_str());
    return;
  }
  else {
    term->tnt = TNT_TERMINAL;
  }
  term->prio = Prio(current.prec, current.assoc);
}

void  jg_prod(const char* lhs_term_name)
{
  current.lhs_term = current.grammar->term_by_name(lhs_term_name);
  if (current.lhs_term->symtype == 0)
    current.lhs_term->symtype = &current.symtype_mgr->default_symtype();
  if (current.lhs_term->tnt == TNT_TERMINAL)
    jg_errorf("Production defined for terminal `%s'", lhs_term_name);
  current.lhs_term->tnt = TNT_NONTERMINAL;
}

void  jg_rhs_part_term(const char* term_name, const char* alias)
{
  current.rhs.parts.push(rhs_part(rhs_part::SYMBOL, term_name, alias));
  if (alias != 0) {
    jacc_term* term = current.grammar->term_by_name(term_name);
    if (0 == term->symtype)
      jg_errorf("Aliasing untyped symbol `%s'", term_name);
  }
}

void  jg_rhs_part_code(const char* action_code, const char* alias)
{
  current.rhs.parts.push(rhs_part(rhs_part::ACTION, action_code, alias));
}

void  jg_rhs_part_prec(const char* prec_term_name)
{
  if (current.rhs.prec_term != 0)
    jg_error("Duplicate %prec definition");

  jacc_term* term = current.grammar->term_by_name(prec_term_name);
  if (term->prio.is_null())
    jg_errorf("Term `%s' has undefined precedence", prec_term_name);
  else
    current.rhs.prec_term = term;
}

void  jg_rhs_part_when(const char* code)
{
  if (current.rhs.when_code != 0)
    jg_error("Duplicate %when definition");
  current.rhs.when_code = code;
}

void  jg_rhs_part_with(const char* code)
{
  if (current.rhs.with_code != 0)
    jg_error("Duplicate %with definition");
  current.rhs.with_code = code;
}

void  jg_rhs(void)
{
  Term::array       rhs_terms;
  jacc_sval::vector svals;
  jacc_grammar&     grammar = *current.grammar;
  const char*       action_code = 0;

  if (grammar.get_start_term() == 0) { 
    grammar.set_start_term(current.lhs_term);
  }

  if (!current.rhs.parts.empty()) {
    rhs_part*     parts_begin = &current.rhs.parts[0];
    rhs_part*     parts_end   = parts_begin + current.rhs.parts.size();

    if (parts_end != parts_begin && parts_end[-1].tag == rhs_part::ACTION) {
      action_code = (--parts_end)->action_code;
    }

    for (rhs_part* part = parts_begin; part != parts_end; part++) {

      // check for duplicate alias:
      if (part->alias != 0) {
        for (rhs_part* p = parts_begin; p != part; ++p) {
          if (p->alias != 0 && 0 == strcmp(p->alias, part->alias)) {
            // the generated code won't compile anyway so just warn:
            jg_errorf("Duplicate alias `%s'", part->alias);
            break;
          }
        }
      }
      jacc_term* term;
      if (part->tag == rhs_part::SYMBOL) {
        term = grammar.term_by_name(part->symbol_name);
      }
      else { // midrule action
        char mra_id[16];
        // note: the leading '@' char is crucial to jacc_prod::is_mra_prod
        sprintf(mra_id, "@%d", ++current.n_mra);
        term = grammar.term_by_name(mra_id);
        ASSERT(term->symtype == 0);
        term->symtype = &current.symtype_mgr->default_symtype();
        Term::array  empty;
        jacc_prod* prod = new jacc_prod(term, empty,
                                        jacc_sval_env(svals, rhs_terms.size()));
        prod->action_code = part->action_code;
        grammar.add_prod(prod);
      }
      if (term->symtype == 0) {
        term->symtype = &current.symtype_mgr->default_symtype();
      }
      rhs_terms.push(term);
      svals.push(jacc_sval(part - parts_begin, term->symtype, part->alias));
    }
  }

  jacc_prod* prod = new jacc_prod(current.lhs_term, rhs_terms,
                                  jacc_sval_env(svals, rhs_terms.size()));
  prod->action_code = action_code;
  prod->when_code = current.rhs.when_code;
  prod->with_code = current.rhs.with_code;
  prod->prec_term = current.rhs.prec_term;

  if (prod->prec_term != 0) {
    prod->prio = prod->prec_term->prio;
  }

  grammar.add_prod(prod);

  current.rhs.reset();
}

void  jg_error(const char* msg)
{
  jg_errorf("%s", msg);
}

void  jg_errorf(const char* fmt, ...)
{
  va_list  argptr;
  va_start(argptr, fmt);
  current.lexer->get_log()->verrorf(fmt, argptr);
  va_end(argptr);
}

int jg_lex(void)
{
  return current.lexer->next_token(&yylval);
}

