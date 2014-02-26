#ifndef UTIL_MINMAX_H
#define UTIL_MINMAX_H

#ifndef OPTION_MINMAX_DEFINED
#define OPTION_MINMAX_DEFINED

template <class T>
inline
const T&  min(const T& a, const T& b)
{
  return  a < b ? a : b;
}

template <class T>
inline
const T&  max(const T& a, const T& b)
{
  return  a > b ? a : b;
}

#endif//OPTION_MINMAX_DEFINED

#endif//UTIL_MINMAX_H
