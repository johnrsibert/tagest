//$Id: fish_rc4.cpp 2768 2011-02-18 02:49:17Z jsibert $
#include <fvar.hpp>
#include <stdlib.h>
#include "par_t.h"
#include "fish_rec.h"
#include "trace.h"

extern ofstream clogf;

void getreturns(dmatrix& returns, const int fleet, const year_month& date, const int cohort, const recaptype_vector& recaps, const int nrec, const int m, const int n);

adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
adstring& adstring_utils_toupper(adstring& ads);
adstring_array& adstring_utils_toupper(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

/**
Sets effort to zero for cells where there are no recorded recaptures.
Used primarily for the "domestic fleet kludge" where there are no spatially
resolved fishing effort records.
\param fleet adstring containing 4 byte fleet code for the fishing fleet
\param date year_month object containing the month to set
\param recaps recaptype_vector containing the observed recaptures
\param recap_recs int containing the number of recapture records in recaps
\param ma object of type par_t (or model_area)
\return int containing number of effort records substituted
\todo
The function  is overloaded for both constant
and variable par_t classes in order to get the dimensions of the model area and
to access the observed recaps. 
This function should either be properly d.
*/
int indexed_regional_fishery_record::
        set_effort_to_recaps(adstring& fleet, year_month& date,
                              recaptype_vector& recaps, int recap_recs, 
                              par_t& ma)
{
  int substitutions = 0;
  const int m =  ma.get_m();
  const int n = ma.get_n();
  dmatrix obs(1, m, ma.get_jlb(), ma.get_jub());

  int efl = adstring_utils_index(fleet_list, fleet);
  int rfl = adstring_utils_index(ma.fleet_names, fleet);
  if ( (rfl < 1) || (efl < 1) )
  {
    cerr << "Illegal fleet name, " << fleet << ", in set_effort_to_recaps(...)" << endl;
    clogf << "Illegal fleet name, " << fleet << ", in set_effort_to_recaps(...)" << endl;
    TTRACE(rfl,ma.fleet_names(rfl))
    TTRACE(efl,fleet_list(efl))
  }


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

