#include <fvar.hpp>
#include <fvar.hpp>
#include <stdlib.h>
#include "par_t.h"
#include "fish_rec.h"
#include "trace.h"
extern ofstream clogf;
adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

void getreturns(dmatrix& returns, int fleet, year_month& date, int cohort,
                recaptype_vector& recaps, int nrec, int m, int n);

int indexed_regional_fishery_record::
        set_effort_to_recaps(adstring& fleet, year_month& date,
                              recaptype_vector& recaps, int recap_recs, 
                              par_t& ma)
{
  int substitutions = 0;
  const int m =  ma.get_m();
  const int n = ma.get_n();
  dmatrix obs(1, m, ma.get_jlb(), ma.get_jub());

  //int efl = fleet_list.index(fleet);
  int efl = adstring_utils_index(fleet_list,fleet);
  //int rfl = ma.fleet_names.index(fleet);
  int rfl = adstring_utils_index(ma.fleet_names,fleet);

  //TTRACE(rfl,ma.fleet_names(rfl))
  //TTRACE(efl,fleet_list(efl))

  const int cohort = 0;

  getreturns(obs, rfl, date, cohort, recaps, recap_recs, m, n);
  
  int year = date.get_year();
  int month = date.get_month_value();
  int n1 = index(efl,year,month);
  int n2 = n1+nrec(efl,year,month)-1;
  if (n2 > n1)
  {
    for (int n = n1; n <= n2; n++)
    {
      regional_fishery_record fr = fishery_records(n);
      
      int ii = fr.i;
      int jj = fr.j;
      //if (obs(ii,jj) > 0.0)
      //  fr.effort = 1.0;
      //else

      // set effort to zero for cell with no observed recaptures
      if (obs(ii,jj) <= 0.0)
        fr.effort = 0.0;
      fishery_records(n) = fr;

      substitutions++;
    }
  }
  return(substitutions);
}

int indexed_regional_fishery_record::
        set_effort_to_recaps(adstring& fleet, recaptype_vector& recaps, 
                             int recap_recs, 
                             par_t& ma)
{
  int tsubs = 0;
  for (int yr = first_year; yr <= last_year; yr++)
  {
    for (int mo = 1; mo <= 12; mo++)
    {
      year_month date(yr,mo);
      int subs = set_effort_to_recaps(fleet, date, recaps, recap_recs, ma);
      tsubs += subs;
      //cout << setw(4) << subs << " for " << fleet << " " << date << endl;
    } 
  }
  return(tsubs);
}
