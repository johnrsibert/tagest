//#include "smoother.h"

// fourth order zero crossing smoother

//#include <iostream.h>
//extern ofstream clogf;

double smoother( double x, double scale)
{
  double y,z;
//  cout << "in smoother scale = " << scale << endl;

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

double dfsmoother( double x, double scale)
{
  double y,z;
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
