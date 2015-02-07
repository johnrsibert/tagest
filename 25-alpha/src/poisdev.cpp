//$Id: poisdev.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <math.h>

double gammln(double xx);
float ran1(int * idum);
#ifdef ZTC
  #define PI = 3.141592654
#endif

float poidev(float xm, int* idum)
{
#ifndef __ZTC__
  const double PI = 3.141592654;
#endif
  static float sq, alxm,g, oldm=(-1.0);
  float em, t, y;
  //if (xm < 1e-10)
  //{
    //em = 0.0;
  //}
  //else if (xm < 12.0)
  if (xm < 12.0)
  {
    if (xm != oldm)
    {
      oldm = xm;
      g= exp(-xm);
    }
    em = -1.0;
    t = 1.0;
    do
    {
      ++em;
      t *= ran1(idum);
    } while ( t > g);
  }
  else
  {
    if (xm != oldm)
    {
      oldm = xm;
      sq = sqrt(2.0*xm);
      alxm = log(xm);
      g = xm*alxm-gammln(xm+1.0);
    }
    do
    {
      do
      {
	y = tan(PI*ran1(idum));
	em = sq*y+xm;
      } while (em < 0.0);
      em = floor(em);
      t = 0.9*(1.0+y*y)*exp(em*alxm-gammln(em+1.0)-g);
    } while (ran1(idum) > t);
  }
  return(em);
}
