#include "precomp.h"

extern ofstream clogf;

void total_tag_penalty(dmatrix& tags, par_t& param, double& pen)
{
  if (param.m_ipar[29] > 0)
  {
    double weight = double(param.m_ipar[29])*1.0e-8;
    pen = norm2(tags)*weight;
  }
} // end of total_tag_penalty

void df_total_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t& param, double& dfpen)
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
