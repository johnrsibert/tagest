//$Id: tridag.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "intersav.h"

extern ofstream clogf;

template <typename VECTOR>
void tridag(const VECTOR& a, const VECTOR& b, const VECTOR& c, const VECTOR& r, VECTOR& u, const VECTOR& bet, const VECTOR& gam, const int nl, const int nu, VECTOR& v)
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
template void tridag<dvector>(const dvector& a, const dvector& b, const dvector& c, const dvector& r, dvector& u, const dvector& bet, const dvector& gam, const int nl, const int nu, dvector& v);
template void tridag<dvar_vector>(const dvar_vector& a, const dvar_vector& b, const dvar_vector& c, const dvar_vector& r, dvar_vector& u, const dvar_vector& bet, const dvar_vector& gam, const int nl, const int nu, dvar_vector& v);

void tridag2(const dvar_vector &a, const dvar_vector &b, const dvar_vector &c, const dvar_vector &r, dvar_vector &u, const dvar_vector &bet, const dvar_vector &gam, const int nl, const int nu, dvar_vector &v)
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
