#ifndef lalr_print_h
#define lalr_print_h

#include <iostream>

class Grammar;
class LALR_table;

enum { // print flags
  PF_DEFAULTS                   = 0x0,
  PF_PRINT_STATE_NON_CORE_ITEMS = 0x1,
  PF_PRINT_ITEM_LA_TERMS        = 0x2,
  PF_PRINT_RESOLVED_CONFLICTS   = 0x4,
  PF_COMPRESS_TABLE             = 0x8
};

void  print_grammar(std::ostream& os, Grammar& grammar);
void  print_LALR_table(std::ostream& os, LALR_table& table);
void  print_setf(int pf);
int   print_getf();

#endif //lalr_print_h
