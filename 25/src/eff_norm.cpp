//$Id: eff_norm.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t.h"
#include "fish_rec.h"

extern ofstream clogf;

// t.set_name(51, "1 => normalize fishing effort to fleet means");
// t.set_name(52, "1 => catchabilities have been adjusted to normalized effort");

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::set_fishing_effort_normalization(indexed_regional_fishery_record& irfr)
{
  if (m_ipar(51) == 1)
  {
     irfr.normalize();
     if (m_ipar(52) == 0)
     {
       int nfleet = get_nfleet();
       for (int f = 1; f <= nfleet; f++)
       {
          q(f) *= irfr.get_mean_effort(f);
       }
       m_ipar(52) = 1;
       cout << "Catchabilities adjusted for normalized effort." << endl;
       clogf << "Catchabilities adjusted for normalized effort." << endl;
     }
  }
  
  else if (m_ipar(51) == 0)
  {
     if (m_ipar(52) == 1)
     {
       int nfleet = get_nfleet();
       for (int f = 1; f <= nfleet; f++)
       {
          q(f) /= irfr.get_mean_effort(f);
       }
       m_ipar(52) = 0;
     }
  }
  
  else
  {
     cerr << "Illegal switch (" << m_ipar(51) 
          << ") passed to set_fishing_effort_normailzation" << endl;
     clogf << "Illegal switch (" << m_ipar(51) 
           << ") passed to set_fishing_effort_normailzation" << endl;
     exit(1);
  }

}
template void par_t<d3_array,dmatrix,dvector,double>::set_fishing_effort_normalization(indexed_regional_fishery_record& irfr);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::set_fishing_effort_normalization(indexed_regional_fishery_record& irfr);
