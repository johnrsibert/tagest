//$Id: dfsigcmp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfsigma_comp(par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& dfparam, dmatrix& dfsigma, year_month& date)
{
  int i, j, k;
  int _m = dfparam.get_m();
  //int n = dfparam.n;
  ivector& jlb = dfparam.get_jlb();
  ivector& jub = dfparam.get_jub();
	int season = dfparam.get_season(date);
  dvector& dfug = dfparam.get_usergridSigma(season);
  /*
  for (j=n+1; j>=0; j--)
  {
    //sigma[m+1][j] = sigma[m][j];
    dfsigma[m][j] += dfsigma[m+1][j];
    dfsigma[m+1][j] = 0.0;

    //sigma[0][j] = sigma[1][j];
    dfsigma[1][j] += dfsigma[0][j];
    dfsigma[0][j] = 0.0;
  }
  */

  for (i=_m; i>=1; i--)
  {
    int j1 = jlb(i);
    int jn = jub(i);
//    //sigma[i][jn+1] = sigma[i][jn];          // <--- these
//    dfsigma[i][jn] += dfsigma[i][jn+1];       // <---
//    dfsigma[i][jn+1] = 0.0;                   // <--- six

//    //sigma[i][j1-1] = sigma[i][j1];          // <---
//    dfsigma[i][j1] += dfsigma[i][j1-1];       // <--- lines
//    dfsigma[i][j1-1] = 0.0;                   // <---

    for (j=jn; j>=j1; j--)
    {
      k = dfparam.get_gridmap(i, j);

      //sigma[i][j] = ug[k].sigma;
      dfug[k] += dfsigma[i][j];
      dfsigma[i][j] = 0.0;

    }
  }
}
template void dfsigma_comp(par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfsigma, year_month& date);
template<> void dfsigma_comp(par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& dfparam, dmatrix& dfsigma, year_month& date) {}
