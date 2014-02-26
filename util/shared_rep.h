#ifndef UTIL_SHARED_REP_H
#define UTIL_SHARED_REP_H

template <class DATA>
class shared_rep
{
  protected:
    class rep_t : public DATA
    {
      private:
        size_t  share;

        rep_t() : share(0) {;}

        rep_t(const DATA& d) : DATA(d), share(0) {;}

        friend class shared_rep<DATA>;
    };

          DATA&   rep()       { return *_rep; }
    const DATA&   rep() const { return *_rep; }

  private:
    rep_t*  _rep;

    static void  release(rep_t* rep)
    {
      if (rep->share)
        rep->share--;
      else
        delete rep;
    }
    
  public:

    shared_rep()
      {
        _rep = new rep_t();
      }

    shared_rep(const shared_rep& sr)
      {
        (_rep = sr._rep)->share++;
      }

    shared_rep(const DATA& d)
      {
        _rep = new rep_t(d);
      }

    shared_rep&  operator = (const shared_rep& sr)
      {
        release(_rep);
        (_rep = sr._rep)->share++;
        return *this;
      }

    shared_rep&  operator = (const DATA& d)
      {
        release(_rep);
        _rep = new rep_t(d);
        return *this;
      }

   ~shared_rep()
      {
        release(_rep);
      }
};

#endif
