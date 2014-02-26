#ifndef UTIL_EALLOCA_H
#define UTIL_EALLOCA_H

#include <malloc.h>

#ifndef ECOUNT
const ECOUNT = 64;
#endif


template <size_t ESIZE>
class  custom_allocator
{
    union cell_type
    {
      char        bytes[ESIZE];
      cell_type*  next_cell;
    };

    struct  block_type
    {
      block_type* next_block;
      cell_type   cell[ECOUNT];
    };

    // make these two void* to get around BC45 compiler bug:
    static  void*  top_cell;
    static  void*  top_block;

  public:
    static  void* allocate();
    static  void  deallocate(void* p);
    static  bool  is_free_cell(void* p);
    static  bool  is_cell(void* p);
    static  void  free_blocks();
};

template <size_t ESIZE>
void* custom_allocator<ESIZE>::top_cell   = 0;

template <size_t ESIZE>
void* custom_allocator<ESIZE>::top_block  = 0;


template <size_t ESIZE>
void* custom_allocator<ESIZE>::allocate()
{
  if (!top_cell) {
    block_type* new_block = (block_type*)malloc(sizeof(block_type));
    cell_type*  prev_cell = 0;
    for (int i = ECOUNT; 0 < i--; ) {
      new_block->cell[i].next_cell = prev_cell;
      prev_cell = &new_block->cell[i];
    }
    top_cell = prev_cell;
    new_block->next_block = (block_type*)top_block;
    top_block = new_block;
  }

  void* allocated = top_cell;
  top_cell = ((cell_type*)top_cell)->next_cell;
  return allocated;
}


template <size_t ESIZE>
void  custom_allocator<ESIZE>::deallocate(void* p)
{
  cell_type* cell = (cell_type*)p;
  cell->next_cell = (cell_type*)top_cell;
  top_cell = cell;
}


template  <size_t ESIZE>
bool  custom_allocator<ESIZE>::is_cell(void* p)
{
  for (block_type* block = (block_type*)top_block; block != 0; block = block->next_block) {
    if ((char*)&block->cell[0] <= (char*)p &&
                                  (char*)p < (char*)&block->cell[ECOUNT]) {
      // p must point at the very begining of a cell:
      return  ((char*)p - (char*)&block->cell[0]) % ESIZE == 0;
    }
  }
  return  false;
}


template  <size_t ESIZE>
bool  custom_allocator<ESIZE>::is_free_cell(void* p)
{
  for (cell_type* cell = (cell_type*)top_cell; cell != 0; cell = cell->next_cell) {
    if (p == cell) {
      return  true;
    }
  }
  return  false;
}


template  <size_t ESIZE>
void  custom_allocator<ESIZE>::free_blocks()
{
  void* vpb = top_block;
  while (vpb) {
    void* del = vpb;
    vpb = *(void**)vpb;
    free(del);
  }
}

#endif//UTIL_EALLOCA_H
