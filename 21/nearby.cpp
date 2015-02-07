#include <fvar.hpp>
#include <time.h>
//#include <iomanip.h>
#include "trace.h"


float ran1(int * idum);
int effort_nearby(const int ii, const int jj, const dmatrix& effort);

int effort_nearby(const int ii, const int jj, const dmatrix& effort)
{
  int en = 0;
  int r1 = effort.rowmin();
  int r2 = effort.rowmax();

  for (int i = ii-1; i <= ii+1; i++)
  {
    if ( (i >= r1) && (i <= r2) )
    {
      int j1 = effort(i).indexmin();
      int j2 = effort(i).indexmax();
      for (int j = jj-1; j <= jj+1; j++)
      {
        if ( (j >= j1) && (j <= j2) && (effort(i,j) > 0.0) )
           en++;
      }
    }
  }

  return(en);
}

double get_nearby_effort(int& i, int& j, const dmatrix& effort)
{
  //clogf.width(12); // << setw(12);
  //clogf.precision(7);
  static int seed = 0;
  if (seed == 0)
  {
    time_t st;
    time (&st);
    TTRACE(seed,st)
    seed = (int)(st%10000);
    TRACE(seed)
  }

  double test = 0.0;
  int r1 = effort.rowmin();
  int r2 = effort.rowmax();
  //TTRACE(i,j)
  int ii = 0;
  int jj = 0;
  do
  {
    float r =ran1(&seed);
    //TTRACE(r, seed)
    ii = i + ((int)(3.0*r)-1);
    //TTRACE(r, ((int)(3.0*r)-1) )
    if ( (ii >= r1) && (ii <= r2) )
    {
      int j1 = effort(ii).indexmin();
      int j2 = effort(ii).indexmax();
      r =ran1(&seed);
      //TTRACE(r,seed)
      jj = j + ((int)(3.0*r)-1);
      //TTRACE(ii,jj)
      if ( (jj >= j1) && (jj <= j2) )
        test = effort(ii,jj);
    }
    //TRACE(test)
  } while (test <= 0.0);
  i = ii;
  j = jj;
  return (test);
}
