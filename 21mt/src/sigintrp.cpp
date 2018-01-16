#include <fvar.hpp>
#include "trace.h"
extern ofstream clogf;


//dmatrix rowcol_average(const dmatrix& x, const int npoint)
void rowcol_average(dmatrix& x, const int npoint)
{
  if (npoint <= 1)
     return; //(x);
  dmatrix y;
  y.allocate(x);
  int np2 = npoint/2;
  if (2*np2 == npoint)
  {
    cerr << "The number of points in the moving average interpolation must be odd."
         << endl;
    cerr << "You have " << npoint << endl;
    clogf << "The number of points in the moving average interpolation must be odd."
          << endl;
    clogf << "You have " << npoint << endl;
    exit(1);
  }


  int i1 = x.rowmin();
  int i2 = x.rowmax();
  int k, k1, k2;

  double sum = 0.0;
  int den = 0;

  for (int i = i1; i <= i2; i++)
  {
    int j1 = x(i).indexmin();
    int j2 = x(i).indexmax();
    for (int j = j1; j <= j2; j++)
    {
      sum = 0,0;
      den = 0;

      k1 = j - np2;
      k2 = j + np2;
      for (k = k1; k <= k2; k++)
      {
        if ((k >= j1) && (k <= j2))
        {
          sum += x(i,k);
          den ++;
        }
      }

      k1 = i - np2;
      k2 = i + np2;
      for (k = k1; k <= k2; k ++)
      {
        if ( (k >= i1) && (k <= i2) )
        {
           if ((j >= x(k).indexmin()) && (j <= x(k).indexmax())
               && (k != i))
           {
             sum += x(k,j);
             den ++;
           }
        }
      }

      if (den > 1)
        y(i,j) = sum/(double)den;
      else
        y(i,j) = x(i,j);

    }
  }

  //return(y);
  x = y;
}


