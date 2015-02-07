//$Id: dftridag.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>


//extern "C" 
void dfloop1(dvector& v, dvector& dfv, dvector& bet, dvector& dfbet, 
             dvector& w, dvector& dfw, dvector& a, dvector& dfa, dvector& dfr,
             int n1, int n2);

void dftridag(dvector& a, dvector& b, dvector& c,dvector& r,
    dvector& bet, dvector& gam,
    dvector& dfa,dvector& dfb,dvector& dfc,dvector& dfr,
    dvector& dfbet, dvector& dfgam,
    dvector& u, dvector& dfu, int nl, int nu,
    dvector& v, dvector& dfv, dvector& w, dvector& dfw)
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


//extern "C" 
void dfloop1(dvector& v, dvector& dfv, dvector& bet, dvector& dfbet, 
             dvector& w, dvector& dfw, dvector& a, dvector& dfa, dvector& dfr,
             int nl, int nu)
{
  register int j;

  for (j = nu; j >= (nl+1); j--)
//  for (j = nu; j >= nl; j--)
  {
     dfw[j]=dfv[j]*bet[j];   //1.
     dfbet[j]+=dfv[j]*w[j];  //2.
     dfv[j] = 0.0;

     dfv[j-1]-=dfw[j]*a[j];  //3.
     dfa[j]-=dfw[j]*v[j-1];  //4.
     dfr[j]+=dfw[j];         //5.
     dfw[j] = 0.0;
  }
}
