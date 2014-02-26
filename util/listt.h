#ifndef UTIL_LISTT_H
#define UTIL_LISTT_H


template <class DATA> class list_traits
{
  public:
    typedef DATA Data;
    typedef DATA DataIn;

    static  void  destroy_data(Data& , bool ) {}
    static  bool  default_owns_data() { return true; }
};


template <class DATA> class list_traits_ptr : public list_traits<DATA*>
{
  public:
    static  void  destroy_data(DATA* pdata, bool del_data)
      {
        if (del_data) {
          delete pdata;
        }
      }
};

#endif//UTIL_LISTT_H
