#include "precomp.h"

void dfrowcol_average(dmatrix& dfx, const int npoint);
void dfsigma_comp(par_t_reg& dfparam, dmatrix& dfsigma, year_month& ym);
void dfuvinterpolate(dmatrix& dfu, dmatrix& dfv, par_t_reg& param,
                     par_t_reg& dfparam, year_month& ym);

void par_t_reg::dfuvs_comp2(par_t_reg& param, par_t_reg& dfparam, dmatrix& dfu,
                           dmatrix& dfv, dmatrix& dfsigma, year_month& ym)
{
//     sigma = rowcol_average(sigma, m_ipar(36));
    dfrowcol_average(dfsigma, m_ipar(36));

//     sigma_comp(sigma, ym);
     dfsigma_comp(dfparam, dfsigma, ym);
//     uvinterpolate(u, v, ym);
     dfuvinterpolate(dfu, dfv, param, dfparam, ym);
}

