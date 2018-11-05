#ifndef generic_stack_h
#define generic_stack_h

template <typename T, class ALLOCATOR>
class generic_stack
{
  T*  base;
  T*  top;
  T*  end;

public:

  enum { DEFAULT_INIT_SIZE = 128 };

  generic_stack(int init_size=DEFAULT_INIT_SIZE)
    {
      base = (T*)ALLOCATOR().malloc(init_size * sizeof(T));
      top = base;
      end = base + init_size;
    }

  ~generic_stack()
    {
      ALLOCATOR().free(base);
    }

  void  grow()
    {
      size_t new_size = 2 * (end - base);
      T*     new_base = (T*)ALLOCATOR().realloc(base, new_size * sizeof(T));
      if (base != new_base)
        {
          top  = new_base + (top - base);
          end  = new_base + new_size;
          base = new_base;
        }
    }

  void  push(T sym)
    {
      if (top == end)
        grow();
      *top++ = sym;
    }

  void  pop()
    {
      assert(top != base);
      --top;
    }

  void  npop(int n)
    {
      top -= n;
      assert(n >= 0);
      assert(base <= top);
    }

  int  size() const
    {
      return (int)(top - base);
    }

  bool  empty() const
    {
      return top == base;
    }

  void  remove_all_elements()
    {
      top = base;
    }

  T  element_at(int idx)
    {
      assert(0 <= idx && idx < size());
      return base[idx];
    }

  T  peek()
    {
      return top[-1];
    }

  T* top_ptr()
    {
      return top;
    }
};

#endif//generic_stack_h
