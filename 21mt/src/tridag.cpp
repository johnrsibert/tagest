#include "precomp.h"
void tridag(dvector& a, dvector& r,
      dvector& u, dvector& bet, dvector& gam,
      int nl, int nu, dvector& v)
{
  int j;

  // this error condition could be detected when bet is computed
  //if (b[nl] == 0.0)
  //  cerr << "error 1 in TRIDAG\n";

  v[nl]=r[nl]*bet[nl];
  for (j=nl+1;j<=nu;j++)
  {
    v[j] = (r[j]-a[j]*v[j-1])*bet[j];
  }

  u[nu] = v[nu];
  for (j=nu-1;j>=nl;j--)
  {
    u[j] = v[j]-gam[j+1]*u[j+1];
  }
}
