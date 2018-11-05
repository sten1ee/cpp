class generic_stack<T>
{
  T[]  base;
  int  top;
  int  end;

public:

  enum { DEFAULT_INIT_SIZE = 128 };

  generic_stack(int init_size=DEFAULT_INIT_SIZE)
    {
      base = new T[init_size];
      end = (top = 0) + init_size;
    }

  void  grow()
    {
      int new_size = 2 * (end - base);
      T[] new_base = new T[new_size];
      System.arraycopy(base, 0, new_base, 0, new_size);
      base = new_base;
      end  = new_size;
    }

  void  push(T sym)
    {
      if (top == end)
        grow();
      base[top++] = sym;
    }

  void  pop()
    {
      assert(top > 0);
      --top;
    }

  void  npop(int n)
    {
      assert(n >= 0);
      assert(n <= top);
      top -= n;
    }

  int  size() const
    {
      return top;
    }

  bool  empty() const
    {
      return top == 0;
    }

  void  remove_all_elements()
    {
      top = 0;
    }

  T  element_at(int idx)
    {
      assert(0 <= idx && idx < size());
      return base[idx];
    }

  T  peek()
    {
      assert(top > 0);
      return base[top-1];
    }

  T* top_ptr()
    {
      return top;
    }
}
