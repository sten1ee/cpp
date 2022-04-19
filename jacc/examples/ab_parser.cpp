#include "ab_parser.h"
#include <stdio.h>

short** ab_parser::action_table()
{

  static short s0[] =
  {  6,
     2,  -4,  3,   2,
    -1,   0
  };
  static short s1[] =
  {  6,
     3,   2,  4,  -4,
    -1,   0
  };
  static short s2[] =
  {  4,
     2,  -2,
    -1,   0
  };
  static short s3[] =
  {  4,
     2,  -1,
    -1,   0
  };
  static short s4[] =
  {  4,
     4,   6,
    -1,   0
  };
  static short s5[] =
  {  2,
    -1,  -3
  };
  static short *action_tab[] =
  {
    s0  +1, s1  +1, s2  +1, s3  +1, s4  +1, s5  +1
  };
  return action_tab;
}

short** ab_parser::reduce_table()
{

  static short s0[] =
  {   6,
      5,  2,  6,  3,
     -1, -1
  };
  static short s1[] =
  {   4,
      5,  4,
     -1, -1
  };
  static short *reduce_tab[] =
  {
    s0  +1, s1  +1,      0,      0,      0,      0
  };
  return reduce_tab;
}

ab_parser::prod_entry* ab_parser::production_table()
{

  static prod_entry  production_tab[] =
  {
    {  0, 2},{  6, 1},{  5, 3},{  5, 0}
  };
  return production_tab;
}

ab_parser::del_entry* ab_parser::delete_table()
{

  static del_entry  delete_tab[] =
  {
      0,   1,   7,   0, 
  };
  return delete_tab;
}

void ab_parser::delete_pending_symbols()
{
  /* delete the look ahead symbol */
  if (cur_token != 0)
    {
      delete static_cast<ab_sym*>(cur_token);
      cur_token = 0;
    }
  /* delete the symbols accumulated on the stack */
  for (int i = stack.size(), j=0; 0 < --i; )
    delete static_cast<ab_sym*>(stack.top_ptr()[--j]);
  stack.npop(stack.size());
}

lr_symbol*
ab_parser::do_action(int _act, lr_parser& _parser, lr_symbol** _stack_top)
{
  ab_sym* RESULT;

  switch (_act) {
  
  case 0: // accept
  {
    RESULT = (0);
    break;
  }
  case 1: // AB ::= S(s)
  {
    ab_sym* s_sym = static_cast<ab_sym*>(_stack_top[-1]);
    int& s = static_cast<value_ab_sym< int >*>(s_sym)->value;
    ab_sym* result_sym = new ab_sym;

    printf("Result is: %d", s);

    RESULT = result_sym;
    break;
  }
  case 2: // S ::= a S(s) b
  {
    ab_sym* s_sym = static_cast<ab_sym*>(_stack_top[-2]);
    int& s = static_cast<value_ab_sym< int >*>(s_sym)->value;
    value_ab_sym< int >* result_sym = new value_ab_sym< int >;
    int& result = result_sym->value;

    result = s + 1;

    RESULT = result_sym;
    break;
  }
  case 3: // S ::=
  {
    value_ab_sym< int >* result_sym = new value_ab_sym< int >;
    int& result = result_sym->value;

    result = 0;

    RESULT = result_sym;
    break;
  }
  default:
    _parser.report_fatal_error("ab_parser::do_action: Unmatched action number.");
  }

  int  top=0;
  for (unsigned del_word = delete_table()[_act]; del_word != 0; del_word >>= 1) {
    --top;
    if (del_word & 1)
      delete static_cast<ab_sym*>(_stack_top[top]);
  }
  return RESULT;
}

/* PARSER_CODE */
