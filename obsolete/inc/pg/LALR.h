#ifndef PG_LALR_H
#define PG_LALR_H

#include "pg/LR1.h"


class LALR_table : public LR1_table
{
  protected:
    LR1_spot::Set   copy_spots(LR1_spot::Set& spots);
    void            reduce_states(LR1_table& lr1_table, ProgressBar& pb);

  public:
    LALR_table(LR1_table& lr1_table, ProgressBar& pb);
};

#endif//PG_LALR_H
