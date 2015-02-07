//$Id: dminmax.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include <float.h>

#define MAXDOUBLE DBL_MAX

double non_zero_min(dmatrix& dd)
{
  double t = MAXDOUBLE;
  int i1 = dd.rowmin();
  int i2 = dd.rowmax();

  int j1 = dd.colmin();
  int j2 = dd.colmax();

  for(int i = i1; i <= i2; i++)
  {
     for(int j = j1; j <= j2; j++)
     {
       double tt = dd(i,j);
         //if(   ! ((tt < 0.0000000001)  &&  (tt > - 0.0000000001))  && (tt < t)  )
          if(  ( tt != 0.0)   &&  tt < t )
              t = tt;
     }
  }

 return t;
}



double non_zero_max(dmatrix& dd)
{
  double t = -MAXDOUBLE;
  int i1 = dd.rowmin();
  int i2 = dd.rowmax();

  int j1 = dd.colmin();
  int j2 = dd.colmax();

  for(int i = i1; i <= i2; i++)
  {
     for(int j = j1; j <= j2; j++)
     {
       double tt = dd(i,j);
         //if(   ! ((tt < 0.0000000001)  &&  (tt > - 0.0000000001))  && (tt > t)  )
         if( tt != 0.0 && tt > t )
              t = tt;
     }
  }

 return t;
}
