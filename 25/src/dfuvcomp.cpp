//$Id: dfuvcomp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"

/* John: I think we should remove boundary references.  P. */

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfuvcomp(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfu, dmatrix& dfv, int season)
{
  dvector& dfugU = dfparam.get_usergridU(season);
  dvector& dfugV = dfparam.get_usergridV(season);

  double rdx = 30.0/param.get_deltax_eq();
  double rdy = 30.0/param.get_deltay();
  int _m = param.get_m();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();
  for (int i=_m; i>=1; i--)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j=jn; j>=j1; j--)
    {
      int k = param.get_gridmap(i, j);

      //v[i][j] = ug[k].v*rdy;
      dfugV[k] += rdy*dfv[i][j];
      dfv[i][j] = 0.0;


      //u[i][j] = ug[k].u*rdx;
      dfugU[k] += rdx*dfu[i][j];
      dfu[i][j] = 0.0;
    }
  }
}
template void dfuvcomp(par_t<d3_array,dmatrix,dvector,double>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfu, dmatrix& dfv, int season);
template<> void dfuvcomp(par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfu, dmatrix& dfv, int season) {}
