#ifndef SETOPERAT_H
#define SETOPERAT_H

enum SO { //set operation
  SO_AND,
  SO_OR ,
  SO_XOR,
  SO_DIFF
};

template <class SET>
void  set_operation(SO so, const SET& set1, const SET& set2, SET* res);

template <class SET>
bool  set_is_equal(const SET& set1, const SET& set2);

template <class SET>
bool  set_is_subset(const SET& set1, const SET& set2);


template <class SET>
SET  operator - (const SET& set1, const SET& set2)
{
  SET res(!OWNS_DATA);
  set_operation(SO_DIFF, set1, set2, &res);
  return res;
}

template <class SET>
void operator -= (SET& a, const SET& b)
// note: there are potential pitfalls regarding the ownership status
// of the elements of a & b !
{
  bool owns_data = a.owns_data();
  if (owns_data) {
    // prevent a from flushing its contents upon assignment:
    a.owns_data(false);
  }
  a = a - b;
  if (owns_data) {
    // restore a's ownership:
    a.owns_data(true);
  }
}

template <class SET>
SET  operator ^ (const SET& set1, const SET& set2)
{
  SET res(!OWNS_DATA);
  set_operation(SO_XOR, set1, set2, &res);
  return res;
}

template <class SET>
void operator ^= (SET& a, const SET& b)
// note: there are potential pitfalls regarding the ownership status
// of the elements of a & b !
{
  bool owns_data = a.owns_data();
  if (owns_data) {
    // prevent a from flushing its contents upon assignment:
    a.owns_data(false);
  }
  a = a ^ b;
  if (owns_data) {
    // restore a's ownership:
    a.owns_data(true);
  }
}

template <class SET>
SET  operator & (const SET& set1, const SET& set2)
{
  SET res(!OWNS_DATA);
  set_operation(SO_AND, set1, set2, &res);
  return res;
}

template <class SET>
void operator &= (SET& a, const SET& b)
// note: there are potential pitfalls regarding the ownership status
// of the elements of a & b !
{
  bool owns_data = a.owns_data();
  if (owns_data) {
    // prevent a from flushing its contents upon assignment:
    a.owns_data(false);
  }
  a = a & b;
  if (owns_data) {
    // restore a's ownership:
    a.owns_data(true);
  }
}

template <class SET>
SET  operator | (const SET& set1, const SET& set2)
{
  SET res(!OWNS_DATA);
  set_operation(SO_OR, set1, set2, &res);
  return res;
}

template <class SET>
void operator |= (SET& a, const SET& b)
// note: there are potential pitfalls regarding the ownership status
// of the elements of a & b !
{
  bool owns_data = a.owns_data();
  if (owns_data) {
    // prevent a from flushing its contents upon assignment:
    a.owns_data(false);
  }
  a = a | b;
  if (owns_data) {
    // restore a's ownership:
    a.owns_data(true);
  }
}

template <class SET>
void  set_operation(SO so,
                    const SET& set1, const SET& set2, SET* res)
{
  UTIL_ASSERT(res->empty());

  typename SET::const_iterator i1 = set1.begin(), end1 = set1.end();
  typename SET::const_iterator i2 = set2.begin(), end2 = set2.end();

  while (i1 != end1 && i2 != end2) {
    typedef  typename SET::Traits Traits;
    int diff = Traits::compare(Traits::keyof(*i1),
                               Traits::keyof(*i2));
    switch (so) {
      case SO_AND:
        if (diff == 0) goto ADD_SET1_ELEMENT;
        break;

      case SO_XOR:
        if (diff != 0) goto ADD_LESS_ELEMENT;
        break;

      case SO_DIFF:
        if (diff < 0) goto ADD_SET1_ELEMENT;
        break;

      case SO_OR: goto ADD_LESS_ELEMENT;

      default:
        //unknown set operation ?!
      ADD_LESS_ELEMENT:
        if (diff <= 0) {
          ADD_SET1_ELEMENT:
            res->insert_back(*i1);
        }
        else {
          res->insert_back(*i2);
        }
    }
    if (diff <= 0) ++i1;
    if (diff >= 0) ++i2;
  }
  switch (so) {
    case SO_XOR:
    case SO_OR :
      while (i2 != end2) {
        res->insert_back(*i2);
        ++i2;
      }

    case SO_DIFF:
      while (i1 != end1) {
        res->insert_back(*i1);
        ++i1;
      }
  }
}

template <class SET>
bool  set_is_equal(const SET& set1, const SET& set2)
{
  if (set1.size() != set2.size()) {
    return false;
  }

  typename SET::const_iterator i1 = set1.begin(), end1 = set1.end();
  typename SET::const_iterator i2 = set2.begin();

  while (i1 != end1) {
    typedef  typename SET::Traits Traits;
    if (0 != Traits::compare(*i1, *i2)) {
      return false;
    }
    ++i1;
    ++i2;
  }

  return true;
}

template <class SET>
bool  set_is_subset(const SET& set1, const SET& set2)
{
  typename SET::const_iterator i1 = set1.begin(), end1 = set1.end();
  typename SET::const_iterator i2 = set2.begin(), end2 = set2.end();
  size_t  left1 = set1.size();
  size_t  left2 = set2.size();

  while (i1 != end1) {
    while (i2 != end2) {
      if (left1 > left2) {
        return false;
      }
      typedef  typename SET::Traits Traits;
      int diff = Traits::compare(Traits::keyof(*i1),
                                 Traits::keyof(*i2));
      ++i2;
      --left2;
      if (diff == 0) {
        goto NEXT_I1;
      }
      if (diff < 0) {  // no match for *i1 in set2
        return false;
      }
    }
    return false;
  NEXT_I1:
    ++i1;
    --left1;
  }
  return true;
}

#endif
