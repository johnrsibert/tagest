//$Id: coh_reld.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t.h"
#include "recap_t.h"


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int cohort_released(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, recaptype_& recapture_record)
{
  int cap_cohort = recapture_record.cohort;

  if (cap_cohort == 0)
  {
    // recapture cohort is zero only for simulation results from a 
    // pooled tag model; cohort zero in that case is released by
    // definition
    return(1);
  }
  else
  {
    int nrelease = param.get_nrelease();
    for (int i = 1; i <= nrelease; i++)
    {
      if (cap_cohort == param.get_tr_cohort(i))
        return(1);
    }
    return(0);
  }
}
template int cohort_released(par_t<d3_array,dmatrix,dvector,double>& param, recaptype_& recapture_record);
template int cohort_released(par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, recaptype_& recapture_record);
