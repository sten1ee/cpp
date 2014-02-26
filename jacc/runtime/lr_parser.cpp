#include <stdlib.h>
#include "lr_parser.h"

#define LENGTH(ARR) ((ARR)[-1])

void lr_parser::init()
{
  _error_os = &cerr;
  _debug_os = &cerr;
  _error_sync_size = 3;
}

lr_parser::lr_parser()
{
  init();
}

lr_parser::lr_parser(scanner* s)
{
  init();
  set_scanner(s);
}

lr_parser::stack::stack(int init_size)
{
  base = (lr_symbol**)malloc(init_size * sizeof(lr_symbol*));
  if (base == 0)
    throw xfatal("Fatal Error: Can't allocate parser stack");
  end = (top = base) + init_size;
}

lr_parser::stack::~stack()
{
  free(base);
}

void  lr_parser::stack::grow()
{
  int         new_size = 2 * (end - base);
  lr_symbol** new_base = (lr_symbol**)realloc(base,
                                              new_size * sizeof(lr_symbol*));
  if (new_base == 0)
    throw xfatal("Fatal Error: Parser stack exhausted");
  if (base != new_base)
    {
      top  = new_base + (top - base);
      end  = new_base + (end - base);
      base = new_base;
    }
}

void lr_parser::report_fatal_error(const char* message, lr_symbol* info)
{
  /* use the normal error message reporting to put out the message */
  report_error(message, info);

  /* throw an exception */
  throw xfatal("Fatal Error: Can't recover from previous error(s)");
}

void lr_parser::report_error(const char* message, lr_symbol* info)
{
  error_os() << message;
  if (info != 0 && info->left != -1)
    error_os() << " at character " << info->left << " of input\n";
  else
    error_os() << "\n";
}

void lr_parser::syntax_error(lr_symbol* cur_token)
{
  report_error("Syntax error", cur_token);
}

void lr_parser::unrecovered_syntax_error(lr_symbol* cur_token)
{
  report_fatal_error("Couldn't repair and continue parse", cur_token);
}

short lr_parser::get_action(int state, int sym)
{
  short   tag;
  int     first, last, probe;
  short*  row = action_tab[state];

  /* linear search if we are < 10 entries */
  if (LENGTH(row) < 20)
    for (probe = 0; probe < LENGTH(row); probe++)
      {
        /* is this entry labeled with our Symbol or the default? */
        tag = row[probe++];
        if (tag == sym || tag == -1)
          {
            /* return the next entry */
            return row[probe];
          }
      }
  /* otherwise binary search */
  else
    {
      first = 0;
      last  = (LENGTH(row)-1)/2 - 1;  // leave out trailing default entry
      while (first <= last)
        {
          probe = (first+last)/2;
          if (sym == row[probe*2])
            return row[probe*2+1];
          else if (sym > row[probe*2])
            first = probe+1;
          else
            last = probe-1;
        }

      /* not found, use the default at the end */
      return row[LENGTH(row)-1];
    }

  // shouldn't happened, but if we run off the end we return the
  // default (error == 0) */
  assert(0);
  return 0;
}

short lr_parser::get_reduce(int state, int sym)
{
  short  tag;
  short* row = reduce_tab[state];

  /* if we have a null row we go with the default */
  if (row == 0)
    {
      assert(row != 0);
      return -1;
    }
  for (int probe = 0; probe < LENGTH(row); probe++)
    {
      /* is this entry labeled with our Symbol or the default? */
      tag = row[probe++];
      if (tag == sym || tag == -1)
        {
          /* return the next entry */
          return row[probe];
        }
    }
  /* if we run off the end we return the default (error == -1) */
  assert(row != 0);
  return -1;
}

lr_symbol* lr_parser::parse()
{
  /* set up direct reference to tables to drive the parser */
  production_tab = production_table();
  action_tab     = action_table();
  reduce_tab     = reduce_table();

  /* initialize the action encapsulation object */
  init_actions();

  /* do user initialization */
  user_init();

  /* get the first token */
  cur_token = scan();

  /* push dummy symbol with start state to get us underway */
  stack.remove_all_elements();
  lr_symbol dummy_sym(0, start_state());
  stack.push(&dummy_sym);

  /* continue until accept or fatal error */
  while (true)
    {
      /* Check current token for freshness. */
      assert(-1 == cur_token->parse_state);

      /* current state is always on the top of the stack */

      /* look up action out of the current state with the current input */
      int act = get_action(stack.peek()->parse_state, cur_token->sym);

      /* decode the action -- > 0 encodes shift */
      if (act > 0)
        {
          /* shift to the encoded state by pushing it on the stack */
          cur_token->parse_state = act-1;
          stack.push(cur_token);

          /* advance to the next Symbol */
          cur_token = scan();
        }
      /* if its less than zero, then it encodes a reduce action */
      else if (act < 0)
        {
          act = (-act)-1;

          /* perform the action for the reduce */
          lr_symbol* lhs_sym = do_action(act);

          /* check for accept indication */
          if (lhs_sym == 0)
            {
              return stack.peek();
            }

          /* look up information about the production */

          lhs_sym->sym      = production_tab[act].lhs_sym;
          short handle_size = production_tab[act].rhs_size;

          /* pop the handle off the stack */
          stack.npop(handle_size);

          /* look up the state to go to from the one popped back to */
          act = get_reduce(stack.peek()->parse_state, lhs_sym->sym);

          /* shift to that state */
          lhs_sym->parse_state = act;
          stack.push(lhs_sym);
        }
      /* finally if the entry is zero, we have an error */
      else if (act == 0)
        {
          /* call user syntax error reporting routine */
          syntax_error(cur_token);

          /* try to error recover */
          switch (error_recovery(false))
            {
            case ERS_FAIL:
              /* if that fails give up with a fatal syntax error */
              unrecovered_syntax_error(cur_token);
              return 0;
            case ERS_SUCCESS:
              break;
            case ERS_ACCEPT:
              return stack.peek();
            default:
              assert(0);
            }
        }
    }

}

/***
lr_parser::ers_t  lr_parser::error_recovery(bool debug)
{
  debug_os() << "# Error recovery is not implemented in this version\n";
  return ERS_FAIL;
}
***/

lr_parser::ers_t  lr_parser::error_recovery(bool debug)
{
  if (debug) debug_os() << "# Attempting error recovery\n";

  /* first pop the stack back into a state that can shift on error and
     do that shift (if that fails, we fail) */
  if (!find_recovery_config(debug))
    {
      if (debug) debug_os() << "# Error recovery fails\n";
      return ERS_FAIL;
    }

  /* read ahead to create lookahead we can parse multiple times */
  read_lookahead();

  /* repeatedly try to parse forward until we make it the required dist */
  while (true)
    {
      /* try to parse forward, if it makes it, bail out of loop */
      if (debug) debug_os() << "# Trying to parse ahead\n";

      if (try_parse_ahead(debug))
        {
          break;
        }

      /* otherwise, consume another symbol and try again */
      restart_lookahead();

      /* if we are now past EOF, we have failed */
      if (lookahead_len <= 0)
        {
          assert(lookahead_len == 0);
          if (debug) debug_os() << "# Error recovery fails at EOF\n";
          return ERS_FAIL;
        }
    }

  /* we have consumed to a point where we can parse forward */
  if (debug) debug_os() << "# Parse-ahead ok, going back to normal parse\n";

  /* do the real parse (including actions) across the lookahead.
     this call will return either ERS_SUCCESS or ERS_ACCEPT */
  return parse_lookahead(debug);
}

bool  lr_parser::find_recovery_config(bool debug)
{
  lr_symbol* error_token;
  int act;

  if (debug) debug_os() << "# Finding recovery state on stack\n";

  /* Remember the right-position of the top symbol on the stack */
  //$int right_pos = ((Symbol)stack.peek()).right;
  //$int left_pos  = ((Symbol)stack.peek()).left;

  /* pop down until we can shift under error symbol */
  while (!shift_under_error())
    {
      /* pop the stack */
      if (debug) debug_os() << "# Pop stack by one, state was # "
                            << stack.peek()->parse_state << '\n';
      //$left_pos = ((Symbol)stack.pop()).left;
      lr_symbol* sym = stack.peek();
      stack.pop();

      /* if we have hit bottom, we fail */
      if (stack.empty())
        {
          if (debug) debug_os() << "# No recovery state found on stack\n";
          return false;
        }

      /* note that this way the bottom stack symbol won't be disposed of */
      dispose_of(sym);
    }

  /* state on top of the stack can shift under error, find the shift */
  act = get_action(stack.peek()->parse_state, error_sym());
  if (debug)
    {
      debug_os() << "# Recover state found (#" << stack.peek()->parse_state
                 << ")\n# Shifting on error to state #" << (act-1) << '\n';
    }

  /* build and shift a special error Symbol */
  error_token = new lr_symbol(error_sym(), act-1);//$, left_pos, right_pos);
  stack.push(error_token);

  return true;
}

void  lr_parser::read_lookahead()
{
  const int ess = error_sync_size(), eof_sym = EOF_sym();
  assert(ess <= sizeof(lookahead) / sizeof(*lookahead));

  got_eof = false;
  lr_symbol* ctok = cur_token;
  cur_token = 0;
  /* fill in the array */
  int i = 0;
  while (true)
    {
      lookahead[i++] = ctok;
      if (ctok->sym == eof_sym)
        {
          got_eof = true;
          break;
        }
      if (i == ess)
        break;

      ctok = scan();
    }
  lookahead_len = i;

  /* start at the beginning */
  lookahead_pos = 0;
}

void  lr_parser::restart_lookahead()
{
  /* move all the existing input over */
  dispose_of(lookahead[0]);
  for (int i = 1; i < lookahead_len; i++)
    lookahead[i-1] = lookahead[i];

  /* read a new symbol into the last spot */
  if (!got_eof)
    {
      lr_symbol* ctok = lookahead[lookahead_len-1] = scan();
      if (ctok->sym == EOF_sym())
        got_eof = true;
    }
  else
    lookahead_len--;

  /* reset our internal position marker */
  lookahead_pos = 0;
}

bool lr_parser::try_parse_ahead(bool debug)
{
  /* create a virtual stack from the real parse stack */
  virtual_stack  vstack(stack);

  /* parse until we fail or get past the lookahead input */
  while (true)
    {
      /* look up the action from the current state (on top of stack) */
      int act = get_action(vstack.top(), cur_err_token()->sym);

      /* if its an error, we fail */
      if (act == 0) return false;

      /* > 0 encodes a shift */
      if (act > 0)
        {
          /* push the new state on the stack */
          vstack.push(act-1);

          if (debug) debug_os() << "# Parse-ahead shifts symbol #"
                                <<  cur_err_token()->sym
                                << " into state #" << (act-1) << '\n';

          /* advance simulated input, if we run off the end, we are done */
          if (!advance_lookahead())
            return true;
        }
      /* < 0 encodes a reduce */
      else
        {
          /* if this is a reduce with the start production we are done */
          if ((-act)-1 == start_production())
            {
              if (debug) debug_os() << "# Parse-ahead accepts\n";
              return true;
            }

          /* get the lhs symbol and the rhs size */
          short lhs      = production_tab[(-act)-1].lhs_sym;
          short rhs_size = production_tab[(-act)-1].rhs_size;

          /* pop handle off the stack */
          for (int i = 0; i < rhs_size; i++)
            vstack.pop();

          if (debug) debug_os() << "# Parse-ahead reduces: handle size = "
                                << rhs_size << " lhs = #" << lhs
                                << " from state #" << vstack.top() << '\n';

          /* look up goto and push it onto the stack */
          vstack.push(get_reduce(vstack.top(), lhs));
          if (debug) debug_os() << "# Goto state #" << vstack.top() << '\n';
        }
    }
}

lr_parser::ers_t  lr_parser::parse_lookahead(bool debug)
{
  /* restart the saved input at the beginning */
  lookahead_pos = 0;

  if (debug)
    {
      debug_os() << "# Reparsing saved input with actions\n"
                 << "# Current symbol is #" << cur_err_token()->sym << '\n'
                 << "# Current state is #"
                 << stack.peek()->parse_state << '\n';
    }

  /* continue until we accept or have read all lookahead input */
  while (true)
    {
      /* current state is always on the top of the stack */

      /* look up action out of the current state with the current input */
      int act = get_action(stack.peek()->parse_state, cur_err_token()->sym);

      /* decode the action -- > 0 encodes shift */
      if (act > 0)
        {
          /* shift to the encoded state by pushing it on the stack */
          cur_err_token()->parse_state = act-1;
          // if (debug) debug_shift(cur_err_token());
          stack.push(cur_err_token());

          /* advance to the next symbol, if there is none, we are done */
          if (!advance_lookahead())
            {
              if (debug) debug_os() << "# Completed reparse\n";

              /* scan next symbol so we can continue parse */
              // BUGFIX by Chris Harris <ckharris@ucsd.edu>:
              //   correct a one-off error by commenting out
              //   this next line.
              cur_token = scan();

              /* go back to normal parser */
              return ERS_SUCCESS;
            }

          if (debug) debug_os() << "# Current symbol is #"
                                << cur_err_token()->sym << '\n';
        }
      /* if its less than zero, then it encodes a reduce action */
      else if (act < 0)
        {
          /* perform the action for the reduce */
          lr_symbol* lhs_sym = do_action((-act)-1);

          if (lhs_sym == 0)
            {
              return ERS_ACCEPT;
            }

          /* look up information about the production */
          lhs_sym->sym      = production_tab[(-act)-1].lhs_sym;
          short handle_size = production_tab[(-act)-1].rhs_size;

          // if (debug) debug_reduce((-act)-1, lhs_sym->sym, handle_size);

          /* pop the handle off the stack */
          stack.npop(handle_size);

          /* look up the state to go to from the one popped back to */
          act = get_reduce(stack.peek()->parse_state, lhs_sym->sym);

          /* shift to that state */
          lhs_sym->parse_state = act;
          stack.push(lhs_sym);

          if (debug) debug_os() << "# Goto state #" << act << '\n';

        }
      /* finally if the entry is zero, we have an error
      (shouldn't happen here, but...)*/
      else // (act == 0)
        {
          assert(0);
        }
    }
}

