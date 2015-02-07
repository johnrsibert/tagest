#include "precomp.h"
#include "intersav.h"

#undef DVECTOR
#define DVECTOR dvector&


extern "C" void dfloop1(DVECTOR v, DVECTOR dfv, DVECTOR bet, DVECTOR dfbet, 
             DVECTOR w, DVECTOR dfw, DVECTOR a, DVECTOR dfa, DVECTOR dfr,
             int n1, int n2);

extern "C" void dftridag(DVECTOR a, DVECTOR b, DVECTOR c,DVECTOR r,
    DVECTOR bet, DVECTOR gam,
    DVECTOR dfa,DVECTOR dfb,DVECTOR dfc,DVECTOR dfr,
    DVECTOR dfbet, DVECTOR dfgam,
    DVECTOR u, DVECTOR dfu, int nl, int nu, intersavetype *savefile,
    DVECTOR v, DVECTOR dfv, DVECTOR w, DVECTOR dfw)
{
  register int j;

  v[nl]=r[nl]*bet[nl];

  for (j=nl+1;j<=nu;j++)
  {
     w[j]=r[j]-a[j]*v[j-1];
     v[j]=w[j]*bet[j];
  }

  u[nu] = v[nu];
  for (j=nu-1;j>=nl;j--)
     u[j] = v[j]-gam[j+1]*u[j+1];

  for (j=nl;j< nu;j++)
  {
//   u[j] = v[j]-gam[j+1]*u[j+1];
     dfv[j] += dfu[j];
     dfgam[j+1] -= u[j+1]*dfu[j];
     dfu[j+1] -= gam[j+1]*dfu[j];
     dfu[j] = 0.0;
  }

//u[nu]=v[nu];
  dfv[nu] += dfu[nu];
  dfu[nu] = 0.0;

  dfloop1(v, dfv, bet, dfbet, w, dfw, a, dfa, dfr, nl, nu);

//  v[1]=r[1]*bet[1];
  dfbet[nl] += r[nl]*dfv[nl];
  dfr[nl] += bet[nl]*dfv[nl];
  dfv[nl] = 0.0;
}//End of dftridag
