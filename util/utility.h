#ifndef UTIL_UTILITY_H
#define UTIL_UTILITY_H

template <class T>
inline
bool  operator != (const T& a, const T& b)
{
  return  !(a == b);
}

#endif//UTIL_UTILITY_H
