//$Id: ruvscomp.cpp 2807 2011-03-02 18:30:52Z jsibert $
#include "par_t_reg.h"

void uvs_read(dmatrix& x, adstring& file_name, int m,
              ivector& jlb, ivector& jub);
void rowcol_average(dmatrix& x, const int npoint);
void rowcol_average(dvar_matrix& x, const int npoint);


extern ofstream clogf;
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::uvs_comp(MATRIX& u, MATRIX& v, MATRIX& sigma, year_month& ym)
{
  //clogf << "uvs_comp for " << ym << endl;

  uvinterpolate(u, v, ym);
  sigma_comp(sigma, ym);
  rowcol_average(sigma, m_ipar(36));
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& ym);
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::uvs_comp(dvar_matrix& u, dvar_matrix& v, dvar_matrix& sigma, year_month& ym);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::uvs_comp(dvar_matrix &uSeasonGrid, dvar_matrix &vSeasonGrid, dvar_matrix &sigmaSeasonGrid, dvar_matrix &u, dvar_matrix &v, dvar_matrix &sigma, const int season) {
	uvinterpolate(uSeasonGrid, vSeasonGrid, u, v, season);
	sigma_comp(sigmaSeasonGrid, sigma, season);
	rowcol_average(sigma, m_ipar(36));
}
