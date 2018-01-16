//$Id: dfsigntr.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "trace.h"
extern ofstream clogf;


void dfrowcol_average(dmatrix& dfx, const int npoint)
{
  if (npoint <= 1)
    return;
  dmatrix dfy;
  dfy.allocate(dfx);

  //x = y;
  dfy = dfx;
  dfx.initialize();

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
 
 
  int i1 = dfx.rowmin();
  int i2 = dfx.rowmax();
  int k, k1, k2;

  //double sum = 0.0;
  double dfsum = 0.0;
  int den = 0;
  //->int dfden = 0;

  for (int i = i2; i >= i1; i--)
  {
    int j1 = dfx(i).indexmin();
    int j2 = dfx(i).indexmax();

    for (int j = j2; j >= j1; j--)
    {

 // ------------- recompute den ----------------------------
      den = 0;

      k1 = i - np2;
      k2 = i + np2;
      for (k = k2; k >= k1; k --)
      {
        if ( (k >= i1) && (k <= i2) )
        {
           if ((j >= dfx(k).indexmin()) && (j <= dfx(k).indexmax())
               && (k != i))
           {
             den ++;
           }
        }
      }

      k1 = j - np2;
      k2 = j + np2;
      for (k = k2; k >= k1; k--)
      {
        if ((k >= j1) && (k <= j2))
        {
          den ++;
        }
      }

 // ------------- end recompute den ----------------------------

      if (den > 1)
      {
        //y(i,j) = sum/(double)den;
        dfsum += dfy(i,j)/(double)den;
        //-> dfy(i,j) = 0.0;
      }
      else
      {
        //y(i,j) = x(i,j);
        dfx(i,j) += dfy(i,j);
        //-> dfy(i,j) = 0.0;
      }

      //->dfden = 0;
      k1 = i - np2;
      k2 = i + np2;
      for (k = k2; k >= k1; k --)
      {
        if ( (k >= i1) && (k <= i2) )
        {
           if ((j >= dfx(k).indexmin()) && (j <= dfx(k).indexmax())
               && (k != i))
           {
             //sum += x(k,j);
             dfx(k,j) += dfsum;
             //->dfden ++;
           }
        }
      }

      k1 = j - np2;
      k2 = j + np2;
      for (k = k2; k >= k1; k--)
      {
        if ((k >= j1) && (k <= j2))
        {
          //sum += x(i,k);
          dfx(i,k) += dfsum;
          //->dfden ++;
        }
      }

		  //sum = 0,0;
      dfsum = 0.0;
      /*
      if (dfden != den)
      {
         cerr << "Error in computing den" << endl;
         clogf << "Error in computing den" << endl;
         TTRACE(i,j)
         TTRACE(den,dfden)
         //exit(1);
      }
      */
    }
  }
}


