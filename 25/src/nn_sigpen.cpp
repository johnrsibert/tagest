//$Id: nn_sigpen.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>

template <typename MATRIX, typename DOUBLE>
void x_curvature(const MATRIX &x, DOUBLE &curv);
template <typename MATRIX, typename DOUBLE>
void y_curvature(const MATRIX &y, DOUBLE &curv);

void dfx_curvature(const dmatrix& x, dmatrix& dfx, double& dfcurv);
void dfy_curvature(const dmatrix& x, dmatrix& dfx, double& dfcurv);

template <typename MATRIX, typename DOUBLE>
void sigpen(const MATRIX& s, const double weight, DOUBLE &penalty)
{
  DOUBLE xcurv,ycurv = 0.0;
  x_curvature<MATRIX, DOUBLE>(s, xcurv);
  y_curvature<MATRIX, DOUBLE>(s, ycurv);
  penalty = weight*(sqrt(xcurv)+sqrt(ycurv));
}
template void sigpen<dmatrix,double>(const dmatrix& s, const double weight, double& penalty);
template void sigpen<dvar_matrix,dvariable>(const dvar_matrix& s, const double weight, dvariable& penalty);

void sigpen(const dmatrix &s, const double weight, double &penalty) {
	sigpen<dmatrix, double>(s, weight, penalty);
}
void dfsigpen(const dmatrix& s, dmatrix& dfs, const double weight, 
              double& dfpenalty)
{
  // recompute
  double xcurv,ycurv = 0.0;
  x_curvature(s, xcurv);
  y_curvature(s, ycurv);

  double dfxcurv,dfycurv = 0.0;

  //penalty = weight*(sqrt(xcurv)+sqrt(ycurv));
  dfycurv += dfpenalty*weight*0.5/sqrt(ycurv);
  dfxcurv += dfpenalty*weight*0.5/sqrt(xcurv);
  //dfycurv += dfpenalty*weight*0.5/(sqrt(ycurv)+1e-20);
  //dfxcurv += dfpenalty*weight*0.5/(sqrt(xcurv)+1e-20);
  dfpenalty = 0.0;


  //y_curvature(s, ycurv);
  dfy_curvature(s, dfs, dfycurv);
  //x_curvature(s, xcurv);
  dfx_curvature(s, dfs, dfxcurv);
  
  //double xcurv,ycurv = 0.0;
  dfycurv = 0.0;
  dfxcurv = 0.0;
}
