#include "ab_parser.h"
#include "ab_sym.h"


const char *pc;

class CharScanner : public scanner
{
  public: lr_symbol* next_token();
};

lr_symbol* CharScanner::next_token()
{
  ab_sym* tok=0;
  while (!tok) {
    switch (*pc++) {
      case 'a' :
        tok = new a_sym(); break;
      case 'b' :
        tok = new b_sym(); break;
      case '\0':
        tok = new ab_sym(ab_sym::eof); pc--; break;
    }
  }
  return tok;
}

int main(int , char* argv[])
{
  pc = argv[1];
  if (!pc)
    pc = "";

  CharScanner cs;
  ab_parser  parser;
  parser.set_scanner(&cs);
  parser.parse();
  return 0;
}
