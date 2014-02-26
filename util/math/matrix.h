#if !defined(math__matrix_h)
#define math__matrix_h

#include <iostream.h>

#include "util/ASSERT.h"
#include "util/math/vector.h"


namespace math {
// ====================================================================

template <class _T, int _M, int _N>
class matrix
{
    public:

      enum {
        M = _M,
        N = _N,
        SIZE = M * N
      };

      typedef typename _T::element_t        element_t;
      typedef          vector<_T,_N>        row_t;
      typedef          matrix<_T,_N,_M>     trans_t;

	protected:
    row_t  row[M];

  public:
    matrix() {;}
  	matrix(const matrix& other);

          bool      operator == (const matrix& other) const;
          bool      operator != (const matrix& other) const;

    const row_t&    operator [] (int i) const { return row[i]; }
          row_t&    operator [] (int i)       { return row[i]; }

          matrix&   operator *= (element_t k);
          matrix    operator -  () const;

          trans_t   trans() const;

          bool      find_first(const element_t& e, int* pi, int* pj) const;
};

template <class T, int M, int N>
matrix<T,M,N>::matrix(const matrix<T,M,N>& other)
{
  for (int i=0; i < M; i++)
    for (int j=0; j < N; j++)
      (*this)[i][j] = other[i][j];
}

template <class T, int M, int N>
vector<T,M>
operator * (const matrix<T,M,N>& A, const vector<T,N>& v)
{
	vector<T,M> r;

  for(int i=0; i<M; i++)
    r[i] = A[i] * v;

  return r;
}

template <class T, int M, int N, int P>
matrix<T,M,P>
operator * (const matrix<T,M,N>& A, const matrix<T,N,P>& B)
{
	matrix<T,M,P> C;

  for(int i=0; i<M; i++) {
  	for(int j=0; j<P; j++) {
      C[i][j] = T::ZERO();
      for(int k=0; k<N; k++)
      	C[i][j] += A[i][k] * B[k][j];
    }
  }

  return C;
}

template <class T, int M, int N>
inline
matrix<T,M,N>
operator *= (matrix<T,M,N>& A, const matrix<T,N,N>& B)
{
  A = A * B;
  return A;
}

template <class T, int M, int N>
matrix<T,M,N>&
matrix<T,M,N>::operator *=  (element_t k)
{
  for(int i=0; i<M; i++)
  	row[i] *= k;
  return *this;
}
/*
template <class T, int M, int N>
inline
matrix<T,M,N>
matrix<T,M,N>::operator - () const
{
	matrix res(*this);
  res *= -1;
  return res;
}
*/
template <class T, int M, int N>
trans_t
matrix<T,M,N>::trans() const
{
  trans_t res;
  for (int i=0; i < M; i++)
    for (int j=0; j < N; j++)
      res[j][i] = (*this)[i][j];
  return res;
}

template <class T, int N, int N_1>
matrix<T,N_1,N_1>
subm(const matrix<T,N,N>& m, int I, int J)
{
  matrix<T,N_1,N_1> r;

  for (int i=0, di=0; i < N; i++) {
    if (i == I) {
      di = -1;
      continue;
    }
    for (int j=0, dj=0; j < N; j++) {
      if (j == J) {
        dj = -1;
        continue;
      }
      r[i + di][j + dj] = m[i][j];
    }
  }

  return r;
}

template <class T, int M, int N>
bool matrix<T, M, N>::find_first(const element_t& e, int* pi, int* pj) const
{
  for (int i=0; i < M; i++)
    for (int j=0; j < N; j++)
      if (e == (*this)[i][j]) {
        *pi = i;
        *pj = j;
        return true;
      }

  return false;
}

//============================================================================
// class sub_mat_ref
//============================================================================

template <class T, int N>
class sub_mat_ref
{
  protected:
    typedef          matrix<T,N,N>        matrix_t;
    typedef typename matrix_t::element_t  element_t;

    const matrix_t* matrix;
          ostream*  pos;

    int size;
    int di[N], dj[N];

  public:
    sub_mat_ref(const matrix_t* m, ostream* pos=0);
    sub_mat_ref(const sub_mat_ref& s, int I, int J);

    element_t   operator  ()(int i, int j) const
      {
        ASSERT(i < size);
        ASSERT(j < size);

        return (*matrix)[di[i]][dj[j]];
      }

    element_t   element(int i, int j) const
      {
        ASSERT(i < size);
        ASSERT(j < size);

        return (*matrix)[di[i]][dj[j]];
      }

    element_t   det() const;
};

template <class T, int N>
sub_mat_ref<T,N>::sub_mat_ref(const matrix_t* m, ostream* p)
{
  matrix = m;
  pos  = p;
  size = N;
  for (int i=0; i < N; i++)
    di[i] = dj[i] = i;
}

template <class T, int N>
sub_mat_ref<T,N>::sub_mat_ref(const sub_mat_ref& s, int I, int J)
{
  ASSERT(0 <= I);
  ASSERT(0 <= J);
  ASSERT(I < s.size);
  ASSERT(J < s.size);

  matrix = s.matrix;
  pos = s.pos;

  size = s.size - 1;

  DEBUGIN(memset(di, -1, N * sizeof(di[0])));
  DEBUGIN(memset(dj, -1, N * sizeof(dj[0])));

  for (int i=0; i < I; i++)
    di[i] = s.di[i];
  for (int i=I; i < size; i++)
    di[i] = s.di[i + 1];

  for (int j=0; j < J; j++)
    dj[j] = s.dj[j];
  for (int j=J; j < size; j++)
    dj[j] = s.dj[j + 1];
}

template <class T, int N>
sub_mat_ref<T,N>::element_t
sub_mat_ref<T,N>::det() const
{
  ASSERT(0 < size);

  if (size == 1) {
    if (pos) *pos << element(0,0);
    return element(0,0);
  }

  if (pos) *pos << '(';
  element_t res = T::ZERO();
  for (int j=0; j < size; j++) {
    if (pos) *pos << (j % 2 == 0 ? "+ " : "- ") << element(0,j) << " * ";
    element_t sub_res = element(0,j) * sub_mat_ref<T,N>(*this, 0, j).det();
    if (j % 2 == 0)
      res += sub_res;
    else
      res -= sub_res;
  }
  if (pos) *pos << ')';
  return res;
}

template <class T, int N>
sub_mat_ref<T,N> SMR(const matrix<T,N,N>& m, ostream* pos=0)
{
  return sub_mat_ref<T,N>(&m, pos);
}
//============================================================================
//
//============================================================================
template <class T, int N>
typename T::element_t  Det(const matrix<T,N,N>& m)
{
  return sub_mat_ref<T,N>(&m).det();
}

template <class T, int N>
matrix<T,N,N>  operator ~(const matrix<T,N,N>& m)
{
  matrix<T,N,N>     res;
  T::element_t      det = Det(m);
  ASSERT(det != T::ZERO());
  sub_mat_ref<T,N>  mr(&m);
  for (int i=0; i < N; i++) {
    for (int j=0; j < N; j++) {
      T::element_t r = sub_mat_ref<T,N>(mr, j, i).det() / det;
      res[i][j] = ((i + j) % 2 ? -r : r);
    }
  }
  return res;
}

template <class T, int N>
matrix<T,N,N>  Adj(const matrix<T,N,N>& m)
{
  matrix<T,N,N>     res;
  sub_mat_ref<T,N>  mr(&m);
  for (int i=0; i < N; i++) {
    for (int j=0; j < N; j++) {
      T::element_t r = sub_mat_ref<T,N>(mr, j, i).det();
      res[i][j] = ((i + j) % 2 ? -r : r);
    }
  }
  return res;
}

template <class T, int N>
ostream& operator << (ostream& os, const vector<T,N>& v)
{
//  os << "(";
  for (int i=0; i < N; i++) {
    if (i)
      os << ", ";
    os << v[i];
  }
//  os << ')';
  return os;
}

template <class T, int M, int N>
ostream& operator << (ostream& os, const matrix<T,M,N>& m)
{
  for (int i=0; i < M; i++) 
    os << '\n' << m[i] << ',';

  return os;
}

// ====================================================================
} //namespace math
#endif //math__matrix_h

/*
struct double_matrix
{
  typedef double element_t;

  static element_t ZERO() { return 0; }
};

//subm<double_matrix,3,2>(m3x3, 1, 1);

typedef matrix<double_matrix, 4, 4> Matrix4x4;
typedef matrix<double_matrix, 3, 3> Matrix3x3;
typedef matrix<double_matrix, 2, 2> Matrix2x2;
typedef matrix<double_matrix, 3, 2> Matrix3x2;
typedef matrix<double_matrix, 2, 3> Matrix2x3;


//
//Matrix2x2 m2x2;
//Matrix3x2 m3x2;
//Matrix3x3 m3x3 = m3x2 * m3x2.trans();

void main()
{
  Matrix4x4 m4x4;
  m4x4[0][0] = 1; m4x4[0][1] = 2; m4x4[0][2] = 3; m4x4[0][3] = 4;
  m4x4[1][0] = 1; m4x4[1][1] = 4; m4x4[1][2] = 9; m4x4[1][3] = 16;
  m4x4[2][0] = 1; m4x4[2][1] = 8; m4x4[2][2] = 27;m4x4[2][3] = 64;
  m4x4[3][0] = 1; m4x4[3][1] = 16;m4x4[3][2] = 81;m4x4[3][3] = 256;
  cout << "\ndet = " << SMR(m4x4, &cout).det() << endl;
  Matrix4x4 c4x4 = Adj(m4x4);
  cout << c4x4 << "\ndet = " << Det(c4x4) << endl;
  cout << m4x4 * c4x4 << endl;

  Matrix3x3 m3x3;
  m3x3[0][0] = 1; m3x3[0][1] = 2; m3x3[0][2] = 3;
  m3x3[1][0] = 1; m3x3[1][1] = 2; m3x3[1][2] = 1;
  m3x3[2][0] = 2; m3x3[2][1] = 3; m3x3[2][2] = 2;
  cout << m3x3 << "\ndet = " << Det(m3x3) << endl;
  Matrix3x3 c3x3 = ~m3x3;
  cout << c3x3 << "\ndet = " << Det(c3x3) << endl;
  cout << m3x3 * c3x3 << endl;
}
*/
