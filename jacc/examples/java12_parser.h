#ifndef java12_parser_h
#define java12_parser_h

#include "lr_parser.h"
#include "java12_sym.h"


class java12_parser : public lr_parser, public lr_parser::action_executor
{
    typedef lr_parser::prod_entry  prod_entry;
    typedef unsigned short         del_entry;

  public:
    virtual short**     action_table();
    virtual short**     reduce_table();
    virtual prod_entry* production_table();
    virtual int         start_state()      { return 0; }
    virtual int         start_production() { return 0; }
    virtual int         EOF_sym()          { return 2; }
    virtual int         error_sym()        { return 1; }
    virtual lr_symbol*  do_action(int         _act,
                                  lr_parser&  _parser,
                                  lr_symbol** _stack_top);

            del_entry*  delete_table();
            void        delete_pending_symbols();

    void init()
      {
        set_action_executor(this);
      }

    java12_parser()
      {
        init();
      }

    java12_parser(scanner* s)
      : lr_parser(s)
      {
        init();
      }

   ~java12_parser()
      {
        delete_pending_symbols();
      }


    void  parse()
      {
        lr_parser::parse();
      }

    /* PARSER_INLINE_CODE */
};

#endif //java12_parser_h
