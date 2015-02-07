//$Id: tottagpn.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t.h"

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void total_tag_penalty(const MATRIX& tags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& pen)
{
  if (param.m_ipar[29] > 0) {
    double weight = double(param.m_ipar[29])*1.0e-8;
    pen = norm2(tags)*weight;
  }
}
template void total_tag_penalty(const dmatrix& tags, par_t<d3_array,dmatrix,dvector,double>& param, double& pen);
template void total_tag_penalty(const dvar_matrix& tags, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dvariable& pen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_total_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, double& dfpen)
{
  if (param.m_ipar[29] > 0)
  {
    double weight = double(param.m_ipar[29])*1.0e-8;

//  pen = norm2(tags)*weight;
    dftags += 2.0*tags*weight*dfpen;
    dfpen = 0.0;
//  clogf << __FILE__ << "dftags: " << dftags << endl;
  }
} // end of dftotal_tag_penalty
template void df_total_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<d3_array,dmatrix,dvector,double>& param, double& dfpen);
template void df_total_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, double& dfpen);
