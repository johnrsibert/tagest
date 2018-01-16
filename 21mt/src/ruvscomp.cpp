#include "precomp2.h"
#include "par_treg.h"

void uvs_read(dmatrix& x, adstring& file_name, int m,
              ivector& jlb, ivector& jub);
//dmatrix rowcol_average(const dmatrix& x, const int npoint);
void rowcol_average(dmatrix& x, const int npoint);


extern ofstream clogf;

void par_t_reg::uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& ym)
{
  //clogf << "uvs_comp for " << ym << endl;
  if (m_ipar[96] == 1)
  {
     cerr << "ipar[96] = 1 no longer supported in function" << endl;
     cerr << "void par_t_reg::uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& ym)" << endl;
     exit(1);

     /*
     char month_str[3];
     sprintf(month_str,"%ym", ym.get_month().get_value());
     char year_str[3];
     sprintf(year_str,"%02d",0);
     adstring file_name = adstring(year_str) + month_str;
     uvs_read(u, file_name + ".u", m, jlb, jub);
     uvs_read(v, file_name + ".v", m, jlb, jub);
     uvs_read(sigma, file_name + ".d", m, jlb, jub);
     //??? Should the function have a return here ???
     */
  }
  else if (m_ipar[96] != 0)
  {
     cerr  << "Unkown u,v, sigma comutation mode (" << m_ipar[96]
           << ") in coffcomp." << endl;
     clogf << "Unkown u,v, sigma comutation mode (" << m_ipar[96]
           << ") in coffcomp." << endl;
     exit(1);
  }

  uvinterpolate(u, v, ym);
  sigma_comp(sigma, ym);
  rowcol_average(sigma, m_ipar(36));
}
