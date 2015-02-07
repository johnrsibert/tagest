//$Id: smoother.cpp 2842 2011-08-15 22:35:14Z jsibert $
#include <fvar.hpp>
//#include "smoother.h"

// fourth order zero crossing smoother

//#include <iostream>
//extern ofstream clogf;

 
dvariable smoother(const dvariable x, const double scale)
{
  dvariable z = 0;
  if ( x > scale)
  {
    z=x;
  }
  else if (x < -scale)
  {
    z=x;
  }
  else if (x >= 0)
  {
    double d=3./(scale*scale);
    double e=-2./(scale*scale*scale);
    z=x*x*x*(d+e*x);
  }
  else if (x < 0)
  {
    double d=3./(scale*scale);
    double e=-2./(scale*scale*scale);
    z=x*x*x*(d-e*x);
  }
  return z;
}

/*
double dfsmoother( double x, double scale)
{
  double z = 0;
//  cout << "in dfsmoother scale = " << scale << endl;

  if ( x > scale)
  {
    z=1;
  }
  else if (x < -scale)
  {
    z=1;
  }
  else if (x >= 0)
  {
    double d=3./(scale*scale);
    double e=-2./(scale*scale*scale);
    z=x*x*(3*d+4*e*x);
  }
  else if (x < 0)
  {
    double d=3./(scale*scale);
    double e=-2./(scale*scale*scale);
    z=x*x*(3*d-4*e*x);
  }
  return z;
}
*/
