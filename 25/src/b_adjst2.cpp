//$Id: b_adjst2.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "coff_t.h"

#define ELEM elem

adstring effort_file_name(adstring& fleet, year_month& date);

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::b_adjust(const par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, const MATRIX& z)
{
	const MATRIX& acoff = get_a();
	const MATRIX& raw_bcoff = get_raw_b();
	MATRIX& bcoff = get_b();
	const MATRIX& ccoff = get_c();
	MATRIX& xbet_c = get_xbet();
	MATRIX& xgam_c = get_xgam();

  int n     = param.get_n();

  //bcoff = raw_bcoff + trans(z);

  for (int j=1; j<=n; j++)
  {
    int i1 = bcoff(j).indexmin();
    int i2 = bcoff(j).indexmax();
    //TTRACE(ilb(j),iub(j))
    for (int i=i1; i<=i2; i++)
    {
      bcoff(j,i) = raw_bcoff(j,i) + z(i,j);
      //bcoff(j,i) += z(i,j);
    }
  }

  for (int j=1; j<=n; j++)
  {
    int i1 = bcoff(j).indexmin();
    int i2 = bcoff(j).indexmax();
    xgam_c.ELEM(j,i1) = 0.0;
    xbet_c.ELEM(j,i1) = 1.0/bcoff.ELEM(j,i1);
    i1++;
    for (int i=i1; i<=i2; i++)
    {
      xgam_c.ELEM(j,i) = ccoff.ELEM(j,i-1) * xbet_c.ELEM(j,i-1);
      xbet_c.ELEM(j,i) = 1.0/(bcoff.ELEM(j,i) - acoff.ELEM(j,i) * xgam_c.ELEM(j,i));
    }
  }
}
template void coff_t<d3_array,dmatrix,dvector,double>::b_adjust(const par_t<d3_array,dmatrix,dvector,double>& param, const dmatrix& z);
template void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::b_adjust(const par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, const dvar_matrix& z);
