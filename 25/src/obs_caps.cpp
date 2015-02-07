//$Id: obs_caps.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "factoral.h"
#include "par_t.h"
#include "recap_t.h"
#include "trace.h"


void getreturns(dmatrix& returns, const int fleet, const year_month& date, const int cohort, const recaptype_vector& recaps, const int nrec, const int m, const int n);


void obs_recap_comp(dmatrix& total_obs_caps, year_month& date, int cohort,
	     recaptype_vector& recaps, int nrec, par_t<d3_array,dmatrix,dvector,double>& param,
	     ivector& effort_occured)
{
  int m = param.get_m();
  int n = param.get_n();

  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();

  dmatrix obs(1,m, jlb, jub);
  total_obs_caps.initialize();

  int nf = param.get_nfleet();
  for (int f = 1; f <= nf; f++)
  {
    if (effort_occured(f))
    {
      //adstring fleet = param.fleet_names[f];

      getreturns(obs, f, date, cohort, recaps, nrec, m, n);
      if (param.fleet_names[f] == "KP")
      {
	TRACE(date)
	TRACE(param.fleet_names[f])
	TRACE(effort_occured(f))
	TRACE(sum(obs))
      }

      total_obs_caps += obs;
    }
  }
}
