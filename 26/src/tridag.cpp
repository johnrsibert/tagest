//$Id: tridag.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>

extern ofstream clogf;

 
void tridag(const dvector& a, const dvector& b, const dvector& c, const dvector& r, dvector& u, const dvector& bet, const dvector& gam, const int nl, const int nu, dvector& v)
{
  if (b[nl] == 0.0)
    cerr << "error 1 in TRIDAG\n";

  v[nl]=r[nl]*bet[nl];
  for (int j=nl+1;j<=nu;j++)
  {
    v[j] = (r[j]-a[j]*v[j-1])*bet[j];
  }

  u[nu] = v[nu];
  for (int j=nu-1;j>=nl;j--)
  {
    u[j] = v[j]-gam[j+1]*u[j+1];
  }
}

/*
void tridag2(const dvector &a, const dvector &b, const dvector &c, const dvector &r, dvector &u, const dvector &bet, const dvector &gam, const int nl, const int nu, dvector &v)
{
	if (b[nl] == 0.0) {
		cerr << "error 1 in TRIDAG\n";
	}
	v[nl]=r[nl]*bet[nl];
	for (int j=nl+1;j<=nu;j++) {
		v[j] = (r[j]-a[j]*v[j-1])*bet[j];
	}
	u[nu] = v[nu];
	for (int j=nu-1;j>=nl;j--) {
		u[j] = v[j]-gam[j+1]*u[j+1];
	}
}
*/
