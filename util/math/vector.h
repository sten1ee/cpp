#ifndef math__vector_h
#define math__vector_h

namespace math {
// ====================================================================

template <class _T, int _N>
class vector
{
  public:
    typedef _T::element_t element_t;
    enum { N = _N, SIZE = _N };

    element_t  element[N];

    vector() {;}
    vector(const vector& v);

    element_t   operator[] (int i) const { return element[i]; }
    element_t&  operator[] (int i)       { return element[i]; }

    // scalar product:
    element_t   operator * (const vector& v) const;

    vector      operator - () const;

    vector      operator + (const vector& v) const;
    vector      operator - (const vector& v) const;

    vector&     operator +=(const vector& v);
    vector&     operator -=(const vector& v);

    vector&     operator *= (element_t f);
    vector&     operator /= (element_t f);
};

template <class T, int N>
vector<T,N>::vector(const vector<T,N>& v)
{
  for (int i=0; i < N; i++)
    element[i] = v.element[i];
}

template <class T, int N>
vector<T,N>::element_t  vector<T,N>::operator * (const vector<T,N>& v) const
{
  element_t res = T::ZERO();
  for (int i=0; i < N; i++)
    res += element[i] * v.element[i];
  return res;
}
/*
template <class T, int N>
vector<T,N>  vector<T,N>::operator - () const
{
  vector res;
  for (int i=0; i < N; i++)
    res.element[i] = -element[i];
  return res;
}

template <class T, int N>
vector<T,N>  vector<T,N>::operator + (const vector<T,N>& v) const
{
  vector res;
  for (int i=0; i < N; i++)
    res.element[i] = element[i] + v.element[i];
  return res;
}

template <class T, int N>
vector<T,N>  vector<T,N>::operator - (const vector<T,N>& v) const
{
  vector<T,N> res;
  for (int i=0; i < N; i++)
    res.element[i] = element[i] - v.element[i];
  return res;
}

template <class T, int N>
vector<T,N>& vector<T,N>::operator += (const vector& v)
{
  for (int i=0; i < N; i++)
    element[i] += v.element[i];
  return *this;
}

template <class T, int N>
vector<T,N>& vector<T,N>::operator -= (const vector& v)
{
  for (int i=0; i < N; i++)
    element[i] -= v.element[i];
  return *this;
}

template <class T, int N>
vector<T,N>& vector<T,N>::operator *= (element_t f)
{
  for (int i=0; i < N; i++)
    element[i] *= f;
  return *this;
}

template <class T, int N>
vector<T,N>& vector<T,N>::operator /= (element_t f)
{
  for (int i=0; i < N; i++)
    element[i] /= f;
  return *this;
}
*/

// ====================================================================
} // namespace math
#endif //math__vector_h

