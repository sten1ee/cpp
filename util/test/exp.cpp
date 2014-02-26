#include "util/assert.h"
#include <ctype.h>
#include <iostream.h>

#include "util/math/vector.h"
#include "util/math/matrix.h"

using namespace math;

class exp;

extern const exp* stub;

class exp_ref {
  private:
    const exp* ptr;

  public:
    const exp* operator         ->() const { return  ptr; }
    const exp& operator          *() const { return *ptr; }
               operator const exp*() const { return  ptr; }


      exp_ref& operator = (const exp_ref& er);
      bool     operator ==(const exp_ref& o) const;

              ~exp_ref();
               exp_ref();
               exp_ref(const exp_ref& er);
    explicit   exp_ref(const exp*     pe);
};

class ref_counter {
  private:
    int nr;
  public:
    ref_counter() : nr(0) {;}
    ref_counter(const ref_counter& rc) : nr(rc.nr) {;}
   ~ref_counter() { ASSERT(nr == 0); }

    ref_counter& operator == (const ref_counter& rc) { nr = rc.nr; return *this; }

        operator int() const { return nr; }
    int operator ++ ()       { return ++nr; }
    int operator -- ()       { ASSERT(0 < nr); return --nr; }
    int operator ++ (int)    { return nr++; }
    int operator -- (int)    { ASSERT(0 < nr); return nr--; }
};

class exp {
  protected:
    char      s;    // possible values are { '0', '1', 'A', '+', '*', '#' }
    char*     atom;
    exp_ref   a, b; // expression parts (if compound)

    mutable ref_counter nr;

    exp() : s('#'), atom(0), a(0), b(0) {;}

    exp(char _s)                   : s(_s) , atom(0)               { ASSERT(is_nular());}
    exp(char*_a)                   : s('A'), atom(_a)              { ASSERT(is_nular());}
    exp(char _s, exp* _a)          : s(_s) , atom(0), a(_a)        { ASSERT(is_unar()); }
    exp(char _s, exp* _a, exp* _b) : s(_s) , atom(0), a(_a), b(_b) { ASSERT(is_binar());}

    friend exp_ref  C    (int  c);
    friend exp_ref  A    (char* id);
    friend exp_ref  NEG  (exp* a);
    friend exp_ref  SUM  (exp* a, exp* b);
    friend exp_ref  PROD (exp* a, exp* b);

    friend exp_ref  operator - (const exp_ref& er);

  public:

    enum  type { CONST, VAR, NEG = '~', SUM = '+', PROD = '*' };

    bool  is_nular() const { return s == 'A' || s == '0' || s == '1'; }
    bool  is_unar () const { return s == '~'; }
    bool  is_binar() const { return s == '+' || s == '*'; }
    bool  is(int  i) const { ASSERT(i == 0 || i == 1); return i == s - '0'; }
    bool  is_neg  () const { return s == '~'; }

    ref_counter&  nref() const { return nr; }
    ostream&      print_on(ostream& os, char ss=0) const;

    static exp_ref  STUB_REF() { return exp_ref(new exp()); }

    typedef exp_ref element_t;
    static inline element_t ZERO() { return C(0); }

    bool  operator == (const exp& other) const;
};

exp_ref stub_ref = exp::STUB_REF();
const exp* stub = stub_ref;

// ==================================================================== //
// ==================================================================== //
inline
exp_ref& exp_ref::operator = (const exp_ref& er)
{
  ASSERT(ptr);
  if (--ptr->nref() <= 0) {
    ASSERT(0 == ptr->nref());
    delete ptr;
  }
  if (0 != (ptr = er.ptr)) {
    ++ptr->nref();
  }
  return *this;
}

inline
bool     exp_ref::operator ==(const exp_ref& o) const
{
  ASSERT(ptr && o.ptr);
  return (ptr ==  o.ptr
      || *ptr == *o.ptr);
}

inline
exp_ref::~exp_ref()
{
  if (ptr && --ptr->nref() <= 0) {
    ASSERT(0 == ptr->nref());
    delete ptr;
  }
}

inline
exp_ref::exp_ref()
{
  ++(ptr = stub)->nref();
}

inline
exp_ref::exp_ref(const exp_ref& er)
{
  ASSERT(ptr != stub);
  ASSERT(er.ptr);
  ++(ptr = er.ptr)->nref();
}

inline
exp_ref::exp_ref(const exp* pe)
{
  if (0 != (ptr = pe))
    ++ptr->nref();
}

// ==================================================================== //
// ==================================================================== //

exp_ref C(int i)
{
  ASSERT(i == 0 || i == 1);
  return exp_ref(new exp(char('0' + i)));
}

exp_ref A(char* a)
{
  return exp_ref(new exp(a));
}

exp_ref NEG(exp* a)
{
  return exp_ref(new exp((char)exp::NEG, a));
}

exp_ref SUM(exp* a, exp* b)
{
  return exp_ref(new exp(exp::SUM, a, b));
}

exp_ref PROD(exp* a, exp* b)
{
  return exp_ref(new exp(exp::PROD, a, b));
}

exp_ref operator - (const exp_ref& a)
{
  if (a->is_neg())
    return a->a;
  if (a->is(0))
    return a;

  return NEG(a);
}

exp_ref operator + (const exp_ref& a, const exp_ref& b)
{
  if (a->is(0))
    return b;
  if (b->is(0))
    return a;

  return SUM(a, b);
}

exp_ref& operator += (exp_ref& a, const exp_ref& b)
{
  return (a = a + b);
}

exp_ref& operator -= (exp_ref& a, const exp_ref& b)
{
  return (a = a + -b);
}

exp_ref operator * (const exp_ref& a, const exp_ref& b)
{
  if (a->is(0))
    return a;
  if (b->is(0))
    return b;
  if (a->is(1))
    return b;
  if (b->is(1))
    return a;

  return PROD(a, b);
}


ostream&  exp::print_on(ostream& os, char ss) const
{
  switch (s) {
    case '0': return os << '0';
    case '1': return os << '1';
    case 'A': return os << atom;
    case '~': return a->print_on(os << "-", s);
    case '+': if (ss == '+')
                return b->print_on(a->print_on(os, s) << " + ", s);
              else
                return b->print_on(a->print_on(os << "(", s) << " + ", s) << ")";
    case '*': return b->print_on(a->print_on(os, s) << " * ", s);
    case '#': return os << " # ";
    default : ASSERT(0); return os;
  }
}


bool  exp::operator == (const exp& o) const
{
  if (this == &o)
    return true;
  if (s != o.s)
    return false;
  if (is_nular())
    if (s != 'A' || atom == o.atom)
      return true;
    else
      return (atom != 0) && (o.atom != 0) && (strcmp(atom, o.atom) == 0);
  if (is_unar())
    return *a == *o.a;
  if (is_binar())
    return (*a == *o.a) && (*b == *o.b);
  ASSERT(0);
  return true;
}

ostream& operator <<(ostream& os, const exp& e)
{
  return e.print_on(os);
}

ostream& operator <<(ostream& os, const exp* pe)
{
  return pe->print_on(os);
}

template <class T, int M, int N>
bool  check_init(const matrix<T,M,N>& m)
{
  int i, j;
  if (m.find_first(stub_ref, &i, &j)) {
    cerr << "\nmatrix element [" << i << "][" << j << "] not initialized !";
    return false;
  }
  return true;
}
 
typedef vector<exp, 2> ev2_t;
typedef vector<exp, 3> ev3_t;
typedef vector<exp, 4> ev4_t;

typedef matrix<exp, 2, 2> em2_t;
typedef matrix<exp, 3, 3> em3_t;
typedef matrix<exp, 4, 4> em4_t;

void test2()
{
  em2_t m;
  m[0][0] = A("a"); m[0][1] = A("b");
  m[1][0] = A("c"); m[1][1] = A("d");
  cout << "\nm = " << m << endl;
  cout << "\ndet = " << Det(m) << endl;
  em2_t a = Adj(m);
  cout << "\na = " << a << endl;
}

void  test3()
{
  em3_t m;
  m[0][0] = A("a"); m[0][1] = A("b"); m[0][2] = A("p");
  m[1][0] = A("c"); m[1][1] = A("d"); m[1][2] = A("q");
  m[2][0] = C( 0 ); m[2][1] = C( 0 ); m[2][2] = C( 1 );
  cout << "\nm = " << m << endl;
  cout << "\ndet = " << Det(m) << endl;
  em3_t c = Adj(m);
  cout << "\nc = " << c << endl;
}

void test4()
{
/*
a b c p
d e f q
g h i r
0 0 0 1
*/
  em4_t m;
  m[0][0] = A("a"); m[0][1] = A("b"); m[0][2] = A("c"); m[0][3] = A("p");
  m[1][0] = A("d"); m[1][1] = A("e"); m[1][2] = A("f"); m[1][3] = A("q");
  m[2][0] = A("g"); m[2][1] = A("h"); m[2][2] = A("i"); m[2][3] = A("r");
  m[3][0] = C( 0 ); m[3][1] = C( 0 ); m[3][2] = C( 0 ); m[3][3] = C( 1 );
  ASSERT(check_init(m));
  em4_t o;
  o[0][0] = A("o.a"); o[0][1] = A("o.b"); o[0][2] = A("o.c"); o[0][3] = A("o.p");
  o[1][0] = A("o.d"); o[1][1] = A("o.e"); o[1][2] = A("o.f"); o[1][3] = A("o.q");
  o[2][0] = A("o.g"); o[2][1] = A("o.h"); o[2][2] = A("o.i"); o[2][3] = A("o.r");
  o[3][0] = C(  0  ); o[3][1] = C(  0  ); o[3][2] = C(  0  ); o[3][3] = C(  1  );
  ASSERT(check_init(o));

  ev4_t v;
  v[0] = A("pt.x"); v[1] = A("pt.y"); v[2] = A("pt.z"); v[3] = C( 1 );

  cout << "\nm * pt =\n" << m * v;
//  cout << "\n(m * o) * v =\n" << (m * o) * v;
//  cout << "\nm * (o * v) =\n" << m * (o * v);
  cout << "\nm * o =\n" << m * o;

//  cout << "\ndet = " << SMR(m, &cout).det() << endl;
//  cout << "\nDet = " << Det(m) << endl;
//  cout << "\nAdj = " << Adj(m) << endl;
}

void calc_tr4()
{
  em4_t m;
  m[0][0] = A("s0.x"); m[0][1] = A("s0.y"); m[0][2] = A("s0.z"); m[0][3] = C(1);
  m[1][0] = A("s1.x"); m[1][1] = A("s1.y"); m[1][2] = A("s1.z"); m[1][3] = C(1);
  m[2][0] = A("s2.x"); m[2][1] = A("s2.y"); m[2][2] = A("s2.z"); m[2][3] = C(1);
  m[3][0] = A("s3.x"); m[3][1] = A("s3.y"); m[3][2] = A("s3.z"); m[3][3] = C(1);
  ASSERT(check_init(m));

  cout << "\nDet = " << Det(m) << endl;
  cout << "\nAdj = " << Adj(m) << endl;
}

void main()
{
  test4();
//  calc_tr4();
}
