#ifndef MATH_SOLVPOLY_H
#define MATH_SOLVPOLY_H

#define _MATH_CLASS
#define _MATH_FUNC

//----------------------------------------------------------
enum P2 {
  P2_,  //no real roots
  P2_XX,//2 equal real roots
  P2_XY,//2 diff. real roots
};
// Solves: x^2 + p*x + q = 0
P2  _MATH_FUNC  SolvePoly2(double p, double q, double r[2]);

//----------------------------------------------------------
enum P3 {
  P3_XYZ,//3 diff  real roots
  P3_XYY,//3 diff  real, second & third equal
  P3_XXX,//3 equal real roots
  P3_X   //1 real root
};
//  Solves: x^3 + p*x + q = 0
P3  _MATH_FUNC  SolvePoly3(double p, double q, double r[3]);
//  Solves: x^3 + p*x^2 + q*x + r = 0
P3  _MATH_FUNC  SolvePoly3(double p, double q, double r, double root[3]);

//----------------------------------------------------------
enum P4 {
  P4_,
  P4_XY,
  P4_XYZT,
  P4_XX,
  P4_XXXX,
  P4_XXYY,
  P4_XXYZ,
  P4_XXXY
};


class _MATH_CLASS Poly4
{
    double c[4];

    double  searchRootBetween(double a, double b);
    double  searchRootLeftTo (double  a);
    double  searchRootRightTo(double  a);
    double  polishDoubleRoot (double x);
    P4      solveDoubleRoot(double a, double root[4]);

  public:
            Poly4(double c0, double c1, double c2, double c3)
              {
                c[0] = c0; c[1] = c1; c[2] = c2; c[3] = c3;
              }
    double  evalIn(double x) const
              {
                //f(x) = x^4 + c0*x^3 + c1*x^2 + c2*x + c3
                return (((x + c[0])*x + c[1])*x + c[2])*x + c[3];
              }
    double  d_evalIn(double x) const
              {
                //f'(x) = 4*x^3 + 3*c0*x^2 + 2*c1*x + c2
                return ((4*x + 3*c[0])*x + 2*c[1])*x + c[2];
              }
    P4      Solve(double root[4]);
};

inline
P4  SolvePoly4(double c0, double c1, double c2, double c3, double root[4])
  {
    return Poly4(c0, c1, c2, c3).Solve(root);
  }
//----------------------------------------------------------
#endif //MATH_SOLVPOLY_H
