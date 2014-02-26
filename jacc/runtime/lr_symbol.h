#ifndef lr_symbol_h
#define lr_symbol_h

/**
 * Defines the lr_symbol class, which is used to represent all terminals
 * and nonterminals while parsing.  The lexer should pass JACC lr_symbol(s)
 * and JACC returns a lr_symbol.
 *
 * @version last updated: 7/3/96
 * @author  Frank Flannery
 */
/* ****************************************************************
  Class lr_symbol
  what the parser expects to receive from the lexer.
  the token is identified as follows:
  sym:    the symbol type
  parse_state: the parse state.
  left :  is the left position in the original input file
  right:  is the right position in the original input file
******************************************************************/

class lr_symbol
{
  friend class lr_parser;
  friend class er_parser;  

  public:  int   sym;
  private: int   parse_state;

  public:  int   left;  // is the left position in the original input file
  public:  int   right; // is the right position in the original input file

  public:  int   get_sym()         const { return sym; }
  public:  int   get_parse_state() const { return parse_state; }

#ifndef NDEBUG
  private: void  init()
    {
      sym          = -1;
      parse_state  = -1;
      left = right = -1;
    }
#endif

  private: lr_symbol(int s, int state)
    {
#ifndef NDEBUG
      init();
#endif
      sym = s;
      parse_state = state;
    }

  public: lr_symbol(int s)
    {
#ifndef NDEBUG
      init();
#endif
      sym = s;
    }

  public: lr_symbol()
    {
#ifndef NDEBUG
      init();
#endif
    }

  public: virtual ~lr_symbol() {;}
};

#endif // lr_symbol_h
