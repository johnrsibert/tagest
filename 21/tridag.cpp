#include "precomp.h"
#include "intersav.h"

extern int saveinterflag;
extern ofstream clogf;

/* for debugging derivative calculations */
extern double fsave,dfstep,dsave,deriv;
extern iswitch;

extern "C" void tridag(DVECTOR a, DVECTOR b, DVECTOR c, DVECTOR r,
      DVECTOR u, DVECTOR bet, DVECTOR gam,
      int nl, int nu, intersavetype * savefile, DVECTOR v)
{
  int j;

  if (b[nl] == 0.0)
    cerr << "error 1 in TRIDAG\n";

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
