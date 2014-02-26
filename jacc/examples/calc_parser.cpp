#include "calc_parser.h"


short** calc_parser::action_table()
{

  static short
  s0[] =
  { 14,
     1,   2,  2,  -6,  4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s1[] =
  {  4,
    16,  -4,
    -1,   0
  },
  s2[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s3[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s4[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s5[] =
  {  2,
    -1,  -7
  },
  s6[] =
  { 22,
     3,  13,  4,  14,  5,  15,  6,  16,  7,  17,  9,  18, 10,  19,
    11,  20, 12,  21, 16,  -2,
    -1,   0
  },
  s7[] =
  {  4,
     2,  -1,
    -1,   0
  },
  s8[] =
  {  4,
    16,  23,
    -1,   0
  },
  s9[] =
  {  2,
    -1,  -9
  },
  s10[] =
  {  2,
    -1,  -8
  },
  s11[] =
  { 22,
     3,  13,  4,  14,  5,  15,  6,  16,  7,  17,  9,  18, 10,  19,
    11,  20, 12,  21, 15,  24,
    -1,   0
  },
  s12[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s13[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s14[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s15[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s16[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s17[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s18[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s19[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s20[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s21[] =
  {  4,
    16,  34,
    -1,   0
  },
  s22[] =
  { 14,
     1,   2,  2,  -6,  4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s23[] =
  {  2,
    -1, -18
  },
  s24[] =
  {  8,
     5,  15,  6,  16,  7,  17,
    -1, -13
  },
  s25[] =
  {  8,
     5,  15,  6,  16,  7,  17,
    -1, -14
  },
  s26[] =
  {  2,
    -1, -10
  },
  s27[] =
  {  2,
    -1, -11
  },
  s28[] =
  {  2,
    -1, -12
  },
  s29[] =
  { 18,
     3,  13,  4,  14,  5,  15,  6,  16,  7,  17,  9,   0, 10,   0,
    11,   0,
    -1, -15
  },
  s30[] =
  { 18,
     3,  13,  4,  14,  5,  15,  6,  16,  7,  17,  9,   0, 10,   0,
    11,   0,
    -1, -16
  },
  s31[] =
  { 18,
     3,  13,  4,  14,  5,  15,  6,  16,  7,  17,  9,   0, 10,   0,
    11,   0,
    -1, -17
  },
  s32[] =
  { 22,
     3,  13,  4,  14,  5,  15,  6,  16,  7,  17,  9,  18, 10,  19,
    11,  20, 12,  21, 13,  36,
    -1,   0
  },
  s33[] =
  { 14,
     1,   2,  2,  -6,  4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s34[] =
  {  4,
     2,  -5,
    -1,   0
  },
  s35[] =
  { 10,
     4,   3,  8,   4, 14,   5, 17,   6,
    -1,   0
  },
  s36[] =
  {  4,
     2,  -3,
    -1,   0
  },
  s37[] =
  { 20,
     3,  13,  4,  14,  5,  15,  6,  16,  7,  17,  9,  18, 10,  19,
    11,  20, 12,  21,
    -1, -19
  },
 *action_tab[] =
  {
    s0  +1, s1  +1, s2  +1, s3  +1, s4  +1, s5  +1, s6  +1, s7  +1, 
    s8  +1, s9  +1, s10 +1, s11 +1, s12 +1, s13 +1, s14 +1, s15 +1, 
    s16 +1, s17 +1, s18 +1, s19 +1, s20 +1, s21 +1, s22 +1, s23 +1, 
    s24 +1, s25 +1, s26 +1, s27 +1, s28 +1, s29 +1, s30 +1, s31 +1, 
    s32 +1, s33 +1, s34 +1, s35 +1, s36 +1, s37 +1
  };
  return action_tab;
}

short** calc_parser::reduce_table()
{

  static short
  s0[] =
  {   6,
     18,  6, 19,  7,
     -1, -1
  },
  s1[] =
  {   4,
     21,  8,
     -1, -1
  },
  s2[] =
  {   4,
     18,  9,
     -1, -1
  },
  s3[] =
  {   4,
     18, 10,
     -1, -1
  },
  s4[] =
  {   4,
     18, 11,
     -1, -1
  },
  s6[] =
  {   4,
     20, 21,
     -1, -1
  },
  s12[] =
  {   4,
     18, 24,
     -1, -1
  },
  s13[] =
  {   4,
     18, 25,
     -1, -1
  },
  s14[] =
  {   4,
     18, 26,
     -1, -1
  },
  s15[] =
  {   4,
     18, 27,
     -1, -1
  },
  s16[] =
  {   4,
     18, 28,
     -1, -1
  },
  s17[] =
  {   4,
     18, 29,
     -1, -1
  },
  s18[] =
  {   4,
     18, 30,
     -1, -1
  },
  s19[] =
  {   4,
     18, 31,
     -1, -1
  },
  s20[] =
  {   4,
     18, 32,
     -1, -1
  },
  s22[] =
  {   6,
     18,  6, 19, 34,
     -1, -1
  },
  s33[] =
  {   6,
     18,  6, 19, 36,
     -1, -1
  },
  s35[] =
  {   4,
     18, 37,
     -1, -1
  },
 *reduce_tab[] =
  {
    s0  +1, s1  +1, s2  +1, s3  +1, s4  +1,      0, s6  +1,      0, 
         0,      0,      0,      0, s12 +1, s13 +1, s14 +1, s15 +1, 
    s16 +1, s17 +1, s18 +1, s19 +1, s20 +1,      0, s22 +1,      0, 
         0,      0,      0,      0,      0,      0,      0,      0, 
         0, s33 +1,      0, s35 +1,      0,      0
  };
  return reduce_tab;
}

calc_parser::prod_entry* calc_parser::production_table()
{

  static prod_entry  production_tab[] =
  {
    {  0, 2},{ 20, 0},{ 19, 4},{ 21, 0},{ 19, 4},{ 19, 0},{ 18, 1},{ 18, 2},
    { 18, 2},{ 18, 3},{ 18, 3},{ 18, 3},{ 18, 3},{ 18, 3},{ 18, 3},{ 18, 3},
    { 18, 3},{ 18, 3},{ 18, 5}
  };
  return production_tab;
}

calc_parser::del_entry* calc_parser::delete_table()
{

  static del_entry  delete_tab[] =
  {
      0,   0,  15,   0,  15,   0,   1,   3,   3,   7,   7,   7,   7,   7, 
      7,   7,   7,   7,  31, 
  };
  return delete_tab;
}

void calc_parser::delete_pending_symbols()
{
  /* delete the look ahead symbol */
  if (cur_token != 0)
    {
      delete static_cast<calc_sym*>(cur_token);
      cur_token = 0;
    }
  /* delete the symbols accumulated on the stack */
  for (int i = stack.size(), j=0; 0 < --i; )
    delete static_cast<calc_sym*>(stack.top_ptr()[--j]);
  stack.npop(stack.size());
}

lr_symbol*
calc_parser::do_action(int _act, lr_parser& _parser, lr_symbol** _stack_top)
{
  calc_sym* RESULT;

  switch (_act) {
  
  case 0: // accept
  {
    RESULT = (0);
    break;
  }
  case 1: // @1 (midrule action)
  {
    calc_sym* e_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& e = static_cast<value_calc_sym< int >*>(e_sym)->value;
    calc_sym* result_sym = new calc_sym;

    cout << "Result is: " << e << endl;

    RESULT = result_sym;
    break;
  }
  case 2: // session ::= exp(e) @1 SEMI session
  {
    calc_sym* result_sym = new calc_sym;

    // (no user action);

    RESULT = result_sym;
    break;
  }
  case 3: // @2 (midrule action)
  {
    calc_sym* result_sym = new calc_sym;

    cout << "(skipping to `;')" << endl;

    RESULT = result_sym;
    break;
  }
  case 4: // session ::= error @2 SEMI session
  {
    calc_sym* result_sym = new calc_sym;

    // (no user action);

    RESULT = result_sym;
    break;
  }
  case 5: // session ::=
  {
    calc_sym* result_sym = new calc_sym;

    cout << "Good bye !" << endl;

    RESULT = result_sym;
    break;
  }
  case 6: // exp ::= NUM(n)
  {
    calc_sym* n_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& n = static_cast<value_calc_sym< int >*>(n_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = n;

    RESULT = result_sym;
    break;
  }
  case 7: // exp ::= NOT exp(a)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = !a;

    RESULT = result_sym;
    break;
  }
  case 8: // exp ::= MINUS exp(a)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = -a;

    RESULT = result_sym;
    break;
  }
  case 9: // exp ::= exp(a) MUL exp(b)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-3]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    calc_sym* b_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& b = static_cast<value_calc_sym< int >*>(b_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = a * b;

    RESULT = result_sym;
    break;
  }
  case 10: // exp ::= exp(a) DIV exp(b)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-3]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    calc_sym* b_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& b = static_cast<value_calc_sym< int >*>(b_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = a / b;

    RESULT = result_sym;
    break;
  }
  case 11: // exp ::= exp(a) REM exp(b)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-3]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    calc_sym* b_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& b = static_cast<value_calc_sym< int >*>(b_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = a % b;

    RESULT = result_sym;
    break;
  }
  case 12: // exp ::= exp(a) PLUS exp(b)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-3]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    calc_sym* b_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& b = static_cast<value_calc_sym< int >*>(b_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = a + b;

    RESULT = result_sym;
    break;
  }
  case 13: // exp ::= exp(a) MINUS exp(b)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-3]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    calc_sym* b_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& b = static_cast<value_calc_sym< int >*>(b_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = a - b;

    RESULT = result_sym;
    break;
  }
  case 14: // exp ::= exp(a) EQ exp(b)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-3]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    calc_sym* b_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& b = static_cast<value_calc_sym< int >*>(b_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = a == b;

    RESULT = result_sym;
    break;
  }
  case 15: // exp ::= exp(a) LESS exp(b)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-3]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    calc_sym* b_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& b = static_cast<value_calc_sym< int >*>(b_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = a < b;

    RESULT = result_sym;
    break;
  }
  case 16: // exp ::= exp(a) GRTR exp(b)
  {
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-3]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    calc_sym* b_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& b = static_cast<value_calc_sym< int >*>(b_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = a > b;

    RESULT = result_sym;
    break;
  }
  case 17: // exp ::= LPAREN exp(e) RPAREN
  {
    calc_sym* e_sym = static_cast<calc_sym*>(_stack_top[-2]);
    int& e = static_cast<value_calc_sym< int >*>(e_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = e;

    RESULT = result_sym;
    break;
  }
  case 18: // exp ::= exp(cond) QMARK exp(a) COLON exp(b)
  {
    calc_sym* cond_sym = static_cast<calc_sym*>(_stack_top[-5]);
    int& cond = static_cast<value_calc_sym< int >*>(cond_sym)->value;
    calc_sym* a_sym = static_cast<calc_sym*>(_stack_top[-3]);
    int& a = static_cast<value_calc_sym< int >*>(a_sym)->value;
    calc_sym* b_sym = static_cast<calc_sym*>(_stack_top[-1]);
    int& b = static_cast<value_calc_sym< int >*>(b_sym)->value;
    value_calc_sym< int >* result_sym = new value_calc_sym< int >;
    int& result = result_sym->value;

    result = (cond ? a : b);

    RESULT = result_sym;
    break;
  }
  default:
    _parser.report_fatal_error("calc_parser::do_action: Unmatched action number.");
  }
  int  top=0;
  for (unsigned del_word = delete_table()[_act]; del_word != 0; del_word >>= 1) {
    --top;
    if (del_word & 1)
      delete static_cast<calc_sym*>(_stack_top[top]);
  }
  return RESULT;
}

/* PARSER_CODE */
