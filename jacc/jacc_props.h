#ifndef jacc_props_h
#define jacc_props_h

#include "util/set.h"

struct jacc_property
{
public:
  enum val_type_t
  {
    NO_VAL,
    INT_VAL,
    STR_VAL
  };

private:
  const char* _key;

  val_type_t  _val_type;

  union {
    int         _int_val;
    const char* _str_val;
  };

public:
  jacc_property(const jacc_property& p)
    : _key(p._key), _val_type(p._val_type)
    {
      ASSERT((void*)&_int_val == (void*)&_str_val);
      if (sizeof(_int_val) >= sizeof(_str_val))
        _int_val = p._int_val;
      else
        _str_val = p._str_val;
    }

  jacc_property(const char* k)
    : _key(k), _val_type(NO_VAL),  _int_val(-1)
    {;}

  jacc_property(const char* k, int iv)
    : _key(k), _val_type(INT_VAL), _int_val(iv)
    {;}

  jacc_property(const char* k, const char* sv)
    : _key(k), _val_type(STR_VAL), _str_val(sv)
    {;}

  const char* key() const
    {
      return _key;
    }

  val_type_t  val_type() const
    {
      return _val_type;
    }

  int  int_val() const
    {
      ASSERT(_val_type == INT_VAL);
      return _int_val;
    }

  const char* str_val() const
    {
      ASSERT(_val_type == STR_VAL);
      return _str_val;
    }

  void  set_val(int iv)
    {
      _val_type = INT_VAL;
      _int_val = iv;
    }

  void  set_val(const char* sv)
    {
      _val_type = STR_VAL;
      _str_val = sv;
    }

  //DEFINE_SET_BY_CSTR_KEY(jacc_property, key());
  struct jacc_property_traits : set_traits< jacc_property >
  {
    typedef const char* DataKey;
    static DataKey keyof(DataIn data) { return data.key(); }
    static int     compare(DataKey k1, DataKey k2) { return strcmp(k1, k2); }
  };
  typedef ::set< jacc_property_traits >  set;

};

/** A property set contains no duplicate keys */
struct jacc_property_set : jacc_property::set
{
  /** Returns reference to the property with the specified key.
      If there is no property with such key - creates one */
  jacc_property& property(const char* key)
  {
    finger f = find(key);
    if (!f)
      insert(f, jacc_property(key));
    return *f;
  }

  jacc_property& property(const string& key)
  {
    return property(key.c_str());
  }

  /** Returns pointer to the property with the specified key.
      If there is no property with such key - return NULL */
  jacc_property* get_property(const char* key)
  {
    finger f = find(key);
    return f ? &(*f) : 0;
  }

  jacc_property* get_property(const string& key)
  {
    return get_property(key.c_str());
  }

  bool  defined(const char* key) const
  {
    return const_cast<jacc_property_set*>(this)->find(key);
  }

  bool  defined(const string& key) const
  {
    return defined(key.c_str());
  }
};

#endif //jacc_props_h
