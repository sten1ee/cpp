typedef unsigned word_t;
typedef void (*memfunc_t)(register word_t* buf, register word_t* ebuf, register volatile word_t areg);
extern  void   read      (register word_t* get, register word_t* eget, register volatile word_t dreg);
extern  void   write     (register word_t* put, register word_t* eput, register volatile word_t sreg);


