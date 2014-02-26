#include "solvpoly.h"
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>

char* root_schema[] = {
  "<>",
  "<XY>",
  "<XYZT>",
  "<XX>",
  "<XXXX>",
  "<XXYY>",
  "<XXYZ>",
  "<XXXY>"
};

// x^4 + c[1]*x^3 + c[2]*x^2 + c[3]*x + c[4]
// eval in c[5]
void  main(int n, char** a) {
  if (n < 6) {
    cout << "Insufficient number of arguments!";
    return;
  }
  double c[5] = {-1., -1., -1., -1., -1.};
  for (int i=0; i < 5; i++) {
    c[i] = atof(a[i + 1]);
  }
  cout << "poly is: x^4 + "
  		 << c[0] << "x^3 +"
       << c[1] << "x^2 +"
       << c[2] << "x +"
       << c[3] << endl;
  cout << "eval pt is: " << c[4] << endl;

  Poly4  p4(c[0], c[1], c[2], c[3]);

  double r[4] = {.0, .0, .0, .0};
  int res = p4.Solve(r);
  cout << "root schema is: " << root_schema[res] << endl;
  for (int i = 0; i < 4; i++) {
  	 char buf[32];
    if (r[i] != 0.) {
    	sprintf(buf, "%.16f", r[i]);
    }
    else {
    	buf[0] = '0';
      buf[1] =  0;
    }
    cout << "\troot[" << i << "] = " << buf << endl;
  }
  cout << "evalIn(" << c[4] << ") = " << p4.evalIn(c[4]);
}
