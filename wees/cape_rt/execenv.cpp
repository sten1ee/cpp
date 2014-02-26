#include "execenv.h"
#include <stdarg.h>
#include <stdio.h>


phase_res_type  ExecEnv::exec(phase_func_type phase_func,
                              ASet&           aset,
                              position_t      beg_pos,
                              position_t      end_pos)
{
  atype_t     old_mind_mask = _mind_mask;
  Label*      old_labels    = _labels;
  int         old_n_labels  = _n_labels;
  position_t  old_end_pos   = _end_pos;

  phase_res_type res = phase_func(*this, aset, beg_pos, end_pos);

  _mind_mask = old_mind_mask;
  _labels    = old_labels;
  _n_labels  = old_n_labels;
  _end_pos   = old_end_pos;

  return res;
}

#ifndef NDEBUG
void  ExecEnv::logf(const char* fmt, ...)
{
  static FILE* log_file = fopen("C:/phase.log", "wb");
  if (log_file != 0) {
    va_list  args;
    va_start(args, fmt);
    vfprintf(log_file, fmt, args);
    va_end(args);
    fflush(log_file);
  }
}
#endif