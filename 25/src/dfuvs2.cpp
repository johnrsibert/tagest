//$Id: dfuvs2.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"

void dfrowcol_average(dmatrix& dfx, const int npoint);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfsigma_comp(par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& dfparam, dmatrix& dfsigma, year_month& ym);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfuvinterpolate(dmatrix& dfu, dmatrix& dfv, par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, year_month& ym);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfuvs_comp2(par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym)
{
//     sigma = rowcol_average(sigma, m_ipar(36));
    dfrowcol_average(dfsigma, m_ipar(36));

//     sigma_comp(sigma, ym);
     dfsigma_comp(dfparam, dfsigma, ym);
//     uvinterpolate(u, v, ym);
     dfuvinterpolate(dfu, dfv, param, dfparam, ym);
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::dfuvs_comp2(par_t_reg<d3_array,dmatrix,dvector,double>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym);
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfuvs_comp2(par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym);
