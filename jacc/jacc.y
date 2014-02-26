%{
#include <malloc.h>    // decl of alloca()
#include "jacc_glue.h" // decl of jg_xxx() functions used in the action code

typedef const char  const_char; // otherwise BC++ messes up
void    yyerror(const_char* msg);
int     yylex(void);
%}

%union {
  const char* str;
}

%token PROP
%token TOKEN TYPE
%token LEFT RIGHT NONASSOC //IPRIO EPRIO
%token PREC WHEN WITH
%token BAR SEMI COMMA DERIVES

%token <str>
  ID           // normal C-style identifier: num_lines
  CODE_STMT    // { } surrounded block of partial valid C++ code
  CODE_DECL    // < > surrounded C++ type: <const char*> <struct Label>
  CODE_EXPR    // ( ) surrounded C++ expression: (infix_prio(term))

%type  <str> ID opt_alias code_string

%% // begin rules
spec
  : prop_list
    type_list
    prec_list
    prod_list
  ;

prop_list
  : prop_list prop
  | /* empty */
  ;

prop
  : PROP ID code_string { jg_prop($2, $3) } opt_semi
  | PROP ID ID          { jg_prop($2, $3) }     semi
  ;

type_list
  : type type_list
  | /* empty */
  ;

type
  : TYPE  ID CODE_DECL { jg_type(TNT_NONTERMINAL, $2, $3) } type_term_list semi
  | TYPE  ID           { jg_type(TNT_NONTERMINAL, $2,  0) } type_term_list semi
  | TYPE     CODE_DECL { jg_type(TNT_NONTERMINAL,  0, $2) } type_term_list semi
  | TOKEN ID CODE_DECL { jg_type(TNT_TERMINAL,    $2, $3) } type_term_list semi
  | TOKEN ID           { jg_type(TNT_TERMINAL,    $2,  0) } type_term_list semi
  | TOKEN    CODE_DECL { jg_type(TNT_TERMINAL,     0, $2) } type_term_list semi
  | TOKEN           ID { jg_type(TNT_TERMINAL,     0,  0);
                         jg_type_term($2)   }          rest_type_term_list semi
  ;

type_term_list
  : type_term_list COMMA ID  { jg_type_term($3) }
  | ID                       { jg_type_term($1) }
  ;

rest_type_term_list
  : COMMA type_term_list
  | /* empty */
  ;

prec_list
  : prec_list prec
  | /* empty */
  ;

prec
  : LEFT     { jg_prec(PREC_LEFT)     } prec_term_list semi
  | RIGHT    { jg_prec(PREC_RIGHT)    } prec_term_list semi
  | NONASSOC { jg_prec(PREC_NONASSOC) } prec_term_list semi
  ;

prec_term_list
  : prec_term_list COMMA ID  { jg_prec_term($3) }
  | ID                       { jg_prec_term($1) }
  ;

prod_list
  : prod_list prod
  | prod
  ;

prod
  : ID DERIVES { jg_prod($1) } rhs_list semi
  ;

rhs_list
  : rhs_list BAR rhs
  | rhs
  ;

rhs
  : rhs_part_list  { jg_rhs() }
  ;

rhs_part_list
  : rhs_part_list rhs_part_term
  | rhs_part_list rhs_part_code
  | rhs_part_list rhs_part_prec
  | rhs_part_list rhs_part_when
  | rhs_part_list rhs_part_with
  | /* empty */
  ;

rhs_part_term
  : ID opt_alias    { jg_rhs_part_term($1, $2) }
  ;

rhs_part_code
  : CODE_STMT       { jg_rhs_part_code($1,  0) }
  ;

rhs_part_prec
  : PREC ID         { jg_rhs_part_prec($2) }
  ;

rhs_part_when
  : WHEN CODE_EXPR  { jg_rhs_part_when($2) }
  ;

rhs_part_with
  : WITH CODE_EXPR  { jg_rhs_part_with($2) }
  ;

opt_alias  // as code expression is 'something enclosed in ()'
  : CODE_EXPR     { $$ = $1 }
  | /* empty */   { $$ =  0 }
  ;

semi
  : SEMI
  | { yyerror("Missing ';'") } error
  ;

opt_semi
  : SEMI
  | /* empty */
  ;

code_string
  : CODE_STMT  { $$ = $1 }
  | CODE_DECL  { $$ = $1 }
  | CODE_EXPR  { $$ = $1 }
  ;
%% // end rules

void  yyerror(const_char* msg)
{
  jg_error(msg);
}

int   yylex(void)
{
  return jg_lex();
}
