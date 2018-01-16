#include "precomp2.h"
#include "par_t.h"
#include "recap_t.h"


int cohort_released(par_t& param, recaptype_& recapture_record)
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
