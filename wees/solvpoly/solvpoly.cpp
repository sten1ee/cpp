#include <complex.h>
#include "solvpoly.h"

# define  __DEBUG 2
# include <checks.h>
const double  EPS = .0001;


const double  EPS2     = EPS * EPS;
const double  ACCURACY = 0.0000001;

//----------------------------------------------------------
P2  _MATH_FUNC  SolvePoly2(double p, double q, double r[2])
{
  //x^2 + p*x + q == 0
  double s = -p/2;
  double d2 = s*s - q;
  if (d2 < -EPS2) {
    return P2_;
  }
  if (d2 <= EPS2) {
    r[0] = r[1] = s;
    return P2_XX;
  }
  double d = sqrt(d2);
  r[0] = s + d;
  r[1] = s - d;
  return P2_XY;
}

//----------------------------------------------------------
P3  _MATH_FUNC  SolvePoly3(double p, double q, double r[3])
{
  //x^3 + p*x + q == 0
  int  dt = -1;
  double d2 = pow(p/3, 3) + pow(q/2, 2);

  if (d2 > 0) { //1 real, 2 complex
    double  d = sqrt(d2);
    double  e = -q/2;
    double  u3= e + d;
    double  v3= e - d;
    if (u3 <= 0) {
      r[0] = -pow(-u3, 1./3.) + -pow(-v3, 1./3.);
    }
    else if (v3 <= 0) {
      r[0] =  pow( u3, 1./3.) + -pow(-v3, 1./3.);
    }
    else {
      r[0] =  pow( u3, 1./3.) +  pow( v3, 1./3.);
    }
    dt = P3_X;
  }
  else if (d2 < 0) { //3 real
    typedef complex complex_double;
    complex_double  d(0, sqrt(-d2));
    complex_double  e(-q/2, 0);
    complex_double  u = pow(e + d, 1./3.);
    complex_double  v = pow(e - d, 1./3.);
    CHECK(imag(u+v) < EPS);
    r[0] = real(u + v);
    complex_double  s = (u - v)*sqrt(3.)/2.;
    complex_double  t(-imag(s), real(s));
    s  = -(u + v)/2.;
    r[1] = real(s + t);
    r[2] = real(s - t);
    dt = P3_XYZ;
  }

  if (fabs(d2) < EPS2) {
    if (dt == -1) { //d2 == 0 exactly
      double e = -q/2;
      double u;
      if (e <= 0) {
        u = -pow(-e, 1./3.);
      }
      else {
        u =  pow( e, 1./3.);
      }
      r[0] = 2*u;
    }
    r[1] = r[2] = -r[0]/2;
    dt = P3_XYY;
  }

  return P3(dt);
}


P3  _MATH_FUNC  SolvePoly3(double  p,  double  q,  double  r, double root[3])
{
  //f(x) = x^3 + p*x^2 + q*x + r
  //substitute x = y - p/3
  double  p2 = p*p;
  P3  dt = SolvePoly3(q - p2/3, p*(2*p2 - 9*q)/27 + r, root);
  root[0] -= p/3;
  root[1] -= p/3;
  root[2] -= p/3;
  return dt;
}

//----------------------------------------------------------
double  Poly4::searchRootBetween(double a, double b)
{
  CHECK(a != b);
  CHECK(evalIn(a) < 0);
  CHECK(0 < evalIn(b));
  double c;
  do {
    c = (a + b)/2;
    if (evalIn(c) < 0)
      a = c;
    else
      b = c;
  } while (fabs(b - a) > ACCURACY); //|| fabs(evalIn(c)) > EPS);
  return c;
}


double  Poly4::searchRootLeftTo(double a)
{
  CHECK(evalIn(a) < 0);
  double step = 1;
  double b = a - step;
  while (evalIn(b) < 0) {
    a = b;
    b -= (step *= 2);
  }
  return  searchRootBetween(a, b);
}


double  Poly4::searchRootRightTo(double a)
{
  CHECK(evalIn(a) < 0);
  double step = 1;
  double b = a + step;
  while (evalIn(b) < 0) {
    a = b;
    b += (step *= 2);
  }
  return  searchRootBetween(a, b);
}


inline
void  swap(double& a, double &b)
{
  double t = a; a = b; b = t;
}


double  Poly4::polishDoubleRoot(double x)
{
  double f = evalIn(x);
  double x_step = f * d_evalIn(x) <= 0 ? ACCURACY : -ACCURACY;

  while (true) {

    double x1 = x + x_step;
    double f1 = evalIn(x1);

    if (f * f1 <= 0) {
      if (fabs(x - x1) <= ACCURACY) {
        return (x + x1) / 2;
      }
      else {
        if (f == 0)
          return x;
        if (f1 == 0)
          return x1;
        if (f < 0)
          return searchRootBetween(x, x1);
        else
          return searchRootBetween(x1, x);
      }
    }
    if (fabs(f1) >= fabs(f)) { // decrease step;
      if (fabs(x - x1) < ACCURACY) {
        return (x + x1) / 2;
      }
      else {
        x_step /= 2;
      }
    }
    else {
      x = x1;
      f = f1;
      x_step *= 2;
    }
  }
}


P4  Poly4::solveDoubleRoot(double a, double root[4])
{
  a = polishDoubleRoot(a);
  //(x - a)^2 == x^2 -2*a*x +a^2 == x^2 + p*x + q
  double  p = -2*a;
  double  q = a*a;
  double  c0 = c[0] -           p; //x^3
  double  c1 = c[1] -    q - c0*p; //x^2

  #if defined(__WARN)
  double  c2 = c[2] - c0*q - c1*p; //x^1
  double  c3 = c[3] - c1*q;        //
  //this == (x - a)^2*(x^2 + c0*x + c1) + (c2*x + c3)
  //c2 & c3 should be very close to 0, more precisely:
  CHECK(fabs(c2*a + c3) <= EPS);
	#endif
  
  //now find the roots of x^2 + c0*x + c1
  root[0] = root[1] = a;
  P2  p2 = SolvePoly2(c0, c1, root + 2);
  switch (p2) {
    case P2_ :
      return P4_XX;

    case P2_XX:
      if (fabs(a - root[2]) < EPS) {
        root[0] = root[1] = root[2] = root[3] = (a + root[2])/2;
        return P4_XXXX;
      }
      else {
        return P4_XXYY;
      }

    case P2_XY   :
      if (fabs(a - root[2]) < EPS) {
        //nothing
      }
      else if (fabs(a - root[3]) < EPS) {
        swap(root[2], root[3]);
      }
      else {
        return  P4_XXYZ;
      }
      root[0] = root[1] = root[2] = (2*a + root[2])/3;
      return P4_XXXY;

    default:
      CHECK(!"case");
      return P4_;
  }
}


P4  Poly4::Solve(double root[4])
{
  double  extr[3]; //array to hold extremal points (roots of the derivate)
  P3  p3 = SolvePoly3((3/4.)*c[0], (2/4.)*c[1], (1/4.)*c[2], extr);
  switch (p3) {
    case P3_X  :
    case P3_XYY:
    case P3_XXX: {
      double y = evalIn(extr[0]);
      if (EPS < y) {
        return P4_;
      }
      if (-EPS <= y) {
        return solveDoubleRoot(extr[0], root);
      }
      root[0] = searchRootLeftTo(extr[0]);
      root[1] = searchRootRightTo(extr[0]);
      return P4_XY;
    }

    case P3_XYZ: {
      //f' has 3 distinct real roots;
      //sort them:
      if (extr[0] > extr[1]) {
        swap(extr[0], extr[1]);
      }
      if (extr[1] > extr[2]) {
        swap(extr[1], extr[2]);
        if (extr[0] > extr[1]) {
          swap(extr[0], extr[1]);
        }
      }

      bool    prevPositive; //was the previous extremum positive?
      int     i = 0;        //#roots found so far
      double  y;

      y = evalIn(extr[0]);
      if (EPS < y) {
        prevPositive = true;
      }
      else if (-EPS <= y) {
        return solveDoubleRoot(extr[0], root);
      }
      else {
        root[i++] = searchRootLeftTo(extr[0]);
        prevPositive = false;
      }

      y = evalIn(extr[1]);
      if (EPS < y) {
        if (!prevPositive) {
          root[i++] = searchRootBetween(extr[0], extr[1]);
          prevPositive = true;
        }
      }
      else if (-EPS <= y) {
        return solveDoubleRoot(extr[1], root);
      }
      else {
        if (prevPositive) {
          root[i++] = searchRootBetween(extr[1], extr[0]);
          prevPositive = false;
        }
      }

      y = evalIn(extr[2]);
      if (EPS < y) {
        if (!prevPositive) {
          root[i++] = searchRootBetween(extr[1], extr[2]);
        }
      }
      else if (-EPS <= y) {
        return solveDoubleRoot(extr[2], root);
      }
      else {
        if (prevPositive) {
          root[i++] = searchRootBetween(extr[2], extr[1]);
        }
        root[i++] = searchRootRightTo(extr[2]);
      }

      if (i == 2)
        return P4_XY;
      if (i == 4)
        return P4_XYZT;

      CHECK(i == 0);
      return P4_;
    }

    default:
      CHECK(!"case");
      return P4_;
  }
}
//----------------------------------------------------------

