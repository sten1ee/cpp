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

public class lr_symbol
{
  public  int   sym;
  private int   parse_state;

  public  int   left;  // is the left position in the original input file
  public  int   right; // is the right position in the original input file

  public  int   get_sym()         const { return sym; }
  public  int   get_parse_state() const { return parse_state; }


  private boolean  init()
    {
      sym          = -1;
      parse_state  = -1;
      left = right = -1;

      return true;
    }


  private lr_symbol(int s, int state)
    {
      assert init();

      sym = s;
      parse_state = state;
    }

  public lr_symbol(int s)
    {
      assert init();

      sym = s;
    }

  public lr_symbol()
    {
      assert init();
    }
}
