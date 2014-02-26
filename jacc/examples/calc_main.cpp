#include <ctype.h>
#include <math.h>
#include <iostream.h>
#include "calc_parser.h"
#include "calc_sym.h"

struct calc_scanner : scanner
{
  istream& is;
  calc_scanner(istream& is) : is(is) {;}

  lr_symbol* next_token();
};

lr_symbol* calc_scanner::next_token()
{
RESCAN:
  while (isspace(is.peek()))
    is.get();

  if (isdigit(is.peek()) || is.peek() == '.')
    {
      int num = 0;
      while (isdigit(is.peek()))
        {
          num *= 10;
          num += is.get() - '0';
        }
      if (is.peek() == '.')
        {
          char buf[256], *put = buf;
          do
            {
              *put++ = (char)is.get();
            }
          while (isdigit(is.peek()) && put < buf + sizeof(buf) - 1);
          *put = 0;
          return new NUM_sym(num + atof(buf));
        }
      return new NUM_sym(num);
    }

  switch (int c = is.get())
    {
  case '(' : return new LPAREN_sym;
  case ')' : return new RPAREN_sym;
  case '+' : return new PLUS_sym;
  case '-' : return new MINUS_sym;
  case '*' : return new MUL_sym;
  case '/' : return new DIV_sym;
  case '%' : return new REM_sym;
  case '>' : return new GRTR_sym;
  case '<' : return new LESS_sym;
  case '=' : return new EQ_sym;
  case '!' : return new NOT_sym;
  case '?' : return new QMARK_sym;
  case ':' : return new COLON_sym;  
  case ';' : return new SEMI_sym;
  case '$' : // artificial EOF sym
  case EOF : return new eof_sym;

  default  : cerr << "calc_scanner: Illegal char '" << (char)c << "'"
                  << " (skipping)" << endl;
             goto RESCAN;
    }
}


void main()
{
  calc_scanner  scanner(cin);
  calc_parser   parser(&scanner);
  try {
  	parser.parse();
  }
  catch (lr_parser::xfatal& exn) {
  	cerr << "Bye bye ..." << endl;
  }
}
