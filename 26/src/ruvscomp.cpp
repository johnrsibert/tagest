//$Id: ruvscomp.cpp 3127 2013-12-05 22:40:36Z jsibert $
#include "par_t_reg.h"
//#include <strstream>

void uvs_read(dmatrix& x, adstring& file_name, int m,
              ivector& jlb, ivector& jub);
void rowcol_average(dmatrix& x, const int npoint);
void rowcol_average(dvar_matrix& x, const int npoint);

void par_t_reg::uvs_comp(dvar_matrix& u, dvar_matrix& v, dvar_matrix& sigma, year_month& ym)
{
  //clogf << "uvs_comp for " << ym << endl;
  const int year = ym.get_year();
  const int month = ym.get_month_value();
  //read_fadmap(fadmap,year,month);
  //int time_step = ym-start_date+1;
  //cout << "time_step: " << time_step << endl;
  //cout << "fadmap_t: " << fadmap_t(time_step) << endl;
  //exit(1);
  if(m_ipar[83] ==1)
  {
  FADGradComp();
  }
  uvinterpolate(u, v, ym);
  sigma_comp(sigma, ym);
  rowcol_average(sigma, m_ipar(36));
}

/* 
void par_t_reg::uvs_comp(dvar_matrix &uSeasonGrid, dvar_matrix &vSeasonGrid, dvar_matrix &sigmaSeasonGrid, dvar_matrix &u, dvar_matrix &v, dvar_matrix &sigma, const int season) {
	uvinterpolate(uSeasonGrid, vSeasonGrid, u, v, season);
	sigma_comp(sigmaSeasonGrid, sigma, season);
	rowcol_average(sigma, m_ipar(36));
}
*/
