//$Id: zmortcmp.cpp 3208 2014-08-30 06:54:21Z jsibert $
#include "par_t.h"
#include "trace.h"

/*
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
total_mort_comp(D3_ARRAY& f_mort1, D3_ARRAY& f_mort0, MATRIX& z, const int cohort)
{
   if (m_ipar(53) == 0)
     total_mort_comp(f_mort1,z, cohort);
   else
   {
     //D3_ARRAY f_mort = f_mort1+f_mort0;
     total_mort_comp(f_mort1, z, cohort);
   }
}
template void par_t<d3_array,dmatrix,dvector,double>::total_mort_comp(d3_array& f_mort1, d3_array& f_mort0, dmatrix& z, const int cohort);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::total_mort_comp(dvar3_array& f_mort1, dvar3_array& f_mort0, dvar_matrix& z, const int cohort);
*/

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
total_mort_comp(D3_ARRAY& f_mort, MATRIX& z)
{
  DOUBLE tmort = mort;
  z = tmort;

  DOUBLE s = 1.0;
  if (m_ipar[20] == 0)
    s = 1.0;
  else if (m_ipar[20] == 1)  // Bills correction
    s = 1.0 - tmort;
  else
  {
     clogf << "Illegal value of m_ipar[20] passed to total_mort_comp" << endl;
     exit(1);
  }

  for (int f=fleet0; f <= nfleet; f++)
  {
    if (m_ipar[20] == 0)
    {
      z += f_mort(f);
    }
    else if (m_ipar[20] == 1)  // Bills correction
    {
      z += f_mort(f)*s;  // i.e. Z = M + F - F*M = M + F*(1 - M)
    }
  }
}
template void par_t<d3_array,dmatrix,dvector,double>::total_mort_comp(d3_array& f_mort, dmatrix& z);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::total_mort_comp(dvar3_array& f_mort, dvar_matrix& z);
