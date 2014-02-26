#include "memfuncs.h"

void read(register word_t* get, register word_t* eget, register volatile word_t dreg)
{
  do {
    dreg = *get;
  } while (++get != eget);
}

void write(register word_t* put, register word_t* eput, register volatile word_t sreg)
{
  do {
    *put = sreg;
  } while (++put != eput);
}

