//$Id: nn_velpen.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>

template <typename MATRIX, typename DOUBLE>
void x_curvature(const MATRIX &x, DOUBLE &curv);
template <typename MATRIX, typename DOUBLE>
void y_curvature(const MATRIX &y, DOUBLE &curv);

void dfx_curvature(const dmatrix& x, dmatrix& dfx, double& dfcurv);
void dfy_curvature(const dmatrix& y, dmatrix& dfx, double& dfcurv);

template <typename MATRIX, typename DOUBLE>
void velpen(const MATRIX &u, const MATRIX &v, const double xweight, const double yweight, DOUBLE &penalty) {
	DOUBLE uxcurv = 0.0,uycurv = 0.0;
	x_curvature<MATRIX, DOUBLE>(u, uxcurv);
	y_curvature<MATRIX, DOUBLE>(u, uycurv);
	penalty = xweight*uxcurv+yweight*uycurv;

	DOUBLE vxcurv = 0.0,vycurv = 0.0;
	x_curvature<MATRIX, DOUBLE>(v, vxcurv);
	y_curvature<MATRIX, DOUBLE>(v, vycurv);
	penalty += xweight*vxcurv+yweight*vycurv;
}
template void velpen<dmatrix,double>(const dmatrix& u, const dmatrix& v, const double xweight, const double yweight, double& penalty);
template void velpen<dvar_matrix,dvariable>(const dvar_matrix& u, const dvar_matrix& v, const double xweight, const double yweight, dvariable& penalty);

void velpen(const dmatrix& u, const dmatrix& v, const double xweight, const double yweight, double& penalty)  {
	velpen<dmatrix, double>(u, v, xweight, yweight, penalty);
}
void dfvelpen(const dmatrix& u, const dmatrix& v, 
              dmatrix& dfu, dmatrix& dfv, 
	      const double xweight, const double yweight, 
              double& dfpenalty)
{
  double dfvxcurv = 0.0;
  double dfvycurv = 0.0;

  //penalty += xweight*vxcurv+yweight*vycurv;
  dfvycurv += dfpenalty*yweight;
  dfvxcurv += dfpenalty*xweight;
  
  //y_curvature(v, vycurv);
  dfy_curvature(v, dfv, dfvycurv);

  //x_curvature(v, vxcurv);
  dfx_curvature(v, dfv, dfvxcurv);

  double dfuxcurv = 0.0;
  double dfuycurv = 0.0;

  //penalty = xweight*uxcurv+yweight*uycurv;
  dfuycurv += dfpenalty*yweight;
  dfuxcurv += dfpenalty*xweight;
  dfpenalty = 0.0;

  //y_curvature(u, uycurv);
  dfy_curvature(u, dfu, dfuycurv);
  //x_curvature(u, uxcurv);
  dfx_curvature(u, dfu, dfuxcurv);

  //double uxcurv,uycurv = 0.0;
  dfuxcurv = 0.0;
  dfuycurv = 0.0;
}
  
