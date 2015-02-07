//$Id: curv_comp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>

template <typename MATRIX, typename DOUBLE>
void y_curvature(const MATRIX& y, DOUBLE& curv)
{
  curv = 0.0;
  int i1 = y.rowmin();
  int i2 = y.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    int j1 = y(i).indexmin() + 1;
    int j2 = y(i).indexmax() - 1;
    for (int j = j1; j <= j2; j++)
    {
      DOUBLE tmp = (y(i,j-1) - 2.0*y(i,j) + y(i,j+1));
      curv += tmp*tmp;
    }
  }
}
template void y_curvature<dmatrix,double>(const dmatrix& y, double& curv);
template void y_curvature<dvar_matrix,dvariable>(const dvar_matrix& y, dvariable& curv);

void y_curvature(const dmatrix &y, double &curv) {
	y_curvature<dmatrix, double>(y,curv);
}
void dfy_curvature(const dmatrix& x, dmatrix& dfx, double& dfcurv)
{
  double dftmp = 0.0;
  int i1 = dfx.rowmin();
  int i2 = dfx.rowmax();
  for (int i = i2; i >= i1; i--)
  {
    int j1 = dfx(i).indexmin() + 1;
    int j2 = dfx(i).indexmax() - 1;
    for (int j = j2; j >= j1; j--)
    {
      // recompute tmp
      double tmp = (x(i,j-1) - 2.0*x(i,j) + x(i,j+1));

      //curv += tmp*tmp;
      dftmp += 2.0*tmp*dfcurv;

      //double tmp = (x(i,j-1) - 2.0*x(i,j) + x(i,j+1));
      dfx(i,j-1) += dftmp;
      dfx(i,j)   -= 2.0*dftmp;
      dfx(i,j+1) += dftmp;
      dftmp = 0.0;
    }
  }
  //curv = 0.0;
  dfcurv = 0.0;
}
template <typename MATRIX, typename DOUBLE>
void x_curvature(const MATRIX& x, DOUBLE& curv)
{
  curv = 0.0;
  int j1 = x.colmin();
  int j2 = x.colmax();
  int i1 = x.rowmin()+1;
  int i2 = x.rowmax()-1;
  for (int j = j1; j <= j2; j++)
  {
    for (int i = i1; i <= i2; i++)
    {
       if ( (j >= x(i-1).indexmin()) && 
            (j <= x(i-1).indexmax()) &&
            (j >= x(i+1).indexmin()) && 
            (j <= x(i+1).indexmax()) )
       {
         DOUBLE tmp = (x(i-1,j) - 2.0*x(i,j) + x(i+1,j));
         curv += tmp*tmp;
       }
     }
  }
}
template void x_curvature<dmatrix,double>(const dmatrix& x, double& curv);
template void x_curvature<dvar_matrix,dvariable>(const dvar_matrix& x, dvariable& curv);

void x_curvature(const dmatrix &x, double &curv) {
	x_curvature<dmatrix, double>(x,curv);
}
void dfx_curvature(const dmatrix& x, dmatrix& dfx, double& dfcurv)
{
  double dftmp = 0.0;
  int j1 = x.colmin();
  int j2 = x.colmax();
  int i1 = x.rowmin()+1;
  int i2 = x.rowmax()-1;
  for (int j = j1; j <= j2; j++)
  {
    for (int i = i1; i <= i2; i++)
    {
       if ( (j >= x(i-1).indexmin()) && 
            (j <= x(i-1).indexmax()) &&
            (j >= x(i+1).indexmin()) && 
            (j <= x(i+1).indexmax()) )
       {
         // recompute tmp
         double tmp = (x(i-1,j) - 2.0*x(i,j) + x(i+1,j));

         //curv += tmp*tmp;
         dftmp += 2.0*tmp*dfcurv;

         //double tmp = (x(i-1,j) - 2.0*x(i,j) + x(i+1,j));
         dfx(i-1,j) += dftmp;
         dfx(i,j)   -= 2.0*dftmp;
         dfx(i+1,j) += dftmp;
         dftmp = 0.0;
       }
     }
  }
  //curv = 0.0;
  dfcurv = 0.0;
}
