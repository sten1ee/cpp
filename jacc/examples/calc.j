%%ERROR_SYNC_SIZE    (1)
%%PARSER_CPP_INCLUDES {
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;
}

%%BEFORE_ACTION {
  /* just do *nothing* before action... */
}

%%AFTER_ACTION {
  for (int idx = RHS_BEG_IDX; idx < RHS_END_IDX; ++idx) {
    calc_sym* sym = RHS_SYM(idx);
  }
}

%token PLUS, MINUS, MUL, DIV, REM, NOT, EQ, LESS, GRTR,
       QMARK, COLON, LPAREN, RPAREN, SEMI;

%token <double> NUM;
%type  <double> exp;

%right QMARK;
%nonassoc EQ, LESS, GRTR;
%left PLUS, MINUS;
%left MUL, DIV, REM;
%left NOT;

session
  : exp(e)
    {cout << "Result is: " << e << endl}
    SEMI
    session
  | error
    {cout << "(skipping to `;')" << endl}
    SEMI
    session
  | /* empty */
    {cout << "Good bye !" << endl}
  ;

exp
  : NUM(n)                          {result = n}
  | NOT   exp(a)                    {result = !a}
  | MINUS exp(a)         %prec NOT  {result = -a}
  | exp(a) MUL    exp(b)            {result = a * b}
  | exp(a) DIV    exp(b)            {result = a / b}
  | exp(a) REM    exp(b)            {result = fmod(a, b)}
  | exp(a) PLUS   exp(b)            {result = a + b}
  | exp(a) MINUS  exp(b)            {result = a - b}
  | exp(a) EQ     exp(b)            {result = a == b}
  | exp(a) LESS   exp(b)            {result = a < b}
  | exp(a) GRTR   exp(b)            {result = a > b}
  | LPAREN exp(e) RPAREN            {result = e}
  | exp(cond) QMARK exp(a)
              COLON exp(b)          {result = (cond ? a : b)}
  ;
