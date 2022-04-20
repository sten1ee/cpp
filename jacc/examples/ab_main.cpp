#include "ab_parser.h"
#include "ab_sym.h"



class CharScanner : public scanner
{
  public:
    const char* const line;
    const char* pc;

    CharScanner(const char* input) : line(input), pc(input) {;}
    
    lr_symbol* next_token()
    {
      const char* start_pc = pc;
      ab_sym* tok=0;

      while (!tok) {
        switch (*pc++) {
          case 'a' :
            tok = new a_sym(); break;
          case 'b' :
            tok = new b_sym(); break;
          case '\0':
          case '$':
            tok = new ab_sym(ab_sym::eof); pc--; break;
          default:
            fprintf(stderr, "[warn] enexpected char '%c' skipped at position %td\n", pc[-1], cpos());
        }
      }

      tok->left = (start_pc - line) + 1;
      tok->right = cpos() + 1;
      return tok;
    }

    ptrdiff_t cpos() const { return pc - line; }
};

int main(int , char* argv[])
{
  const char* input = argv[1];
  CharScanner cs(input ? input : "");
  ab_parser  parser;
  parser.set_scanner(&cs);
  try {
    parser.parse();
    return 0;
  }
  catch (lr_parser::xfatal& exn) {
    fprintf(stderr, "[err] xfatal: %s\n", exn.msg());
    return 1;
  }
}
