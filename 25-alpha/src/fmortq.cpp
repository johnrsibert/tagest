//$Id: fmortq.cpp 3208 2014-08-30 06:54:21Z jsibert $
#include "par_t.h"
#include "trace.h"

/**
\file fmortq.cpp
Functions to compute fishing mortality.
*/

/** Compute fishing mortality from catchability and fishing effort.
\tparam f_mort  3D array over model domain by fleet. On return, the layers contain fishing mortality 
for each fleet.
\f$ F_{fij} = q_f*E_{fij}\f$
\param effort d3_array of observed fishing effort, \f$E_{fij}\f$ over model domain by fleet.
\param date year_month object containing the year and month of fishing effort
*/
/*
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::fish_mort_comp(D3_ARRAY& f_mort, const d3_array& effort, year_month& date)
{
  // q dvector
  for (int f = 1; f <= nfleet;  f++)
  {
    f_mort(f) = q(f)*effort(f);
  }

  if (num_afm)
  {
    cerr << "anomalous fishing mortality no longer supported" << endl;
    exit(1);
    year_month afm_date;
    for (int i = 1; i <= num_afm; i++)
    {
      afm_date.set(afm_yr(i), afm_mo(i));
      if (afm_date == date)
      {
        int ff = afm_fl(i);
        int ii = afm_i(i);
        int jj = afm_j(i);

        f_mort(ff,ii,jj) = afm(i);

      }
    }
  }
}

template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::fish_mort_comp(dvar3_array& f_mort, const d3_array& effort, year_month& date);
template void par_t<d3_array,dmatrix,dvector,double>::fish_mort_comp(d3_array& f_mort, const d3_array& effort, year_month& date);
*/



/** Compute fishing mortality from catchability and fishing effort.
\tparam f_mort1  3D array over model domain by fleet. 
On return, the layers contain fishing mortality for each fleet for recaptures with reported fishing effort.
\f$ F_{fij} = q_f*E_{fij}\f$
\tparam f_mort0  3D array over model domain by fleet. 
On return, the layers contain fishing mortality for each fleet for recaptures with missing fishing effort.
Computation method depends on flags 27, 52, 53
\param effort d3_array of observed fishing effort, \f$E_{fij}\f$ over model domain by fleet.
\param date year_month object containing the year and month of fishing effort
\tparam tag_density matrix over model domain containing predicted tag density.
\param recaps0 d3_array over model domain by fleet containing tag recaptures for which no fishing
effort was reported.
\param mean_effort dvector of mean observed fishing effort by feleet.
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::fish_mort_comp(D3_ARRAY& f_mort, const d3_array& effort)
{
  const int effort_exclusion_flag = m_ipar(54);
  //TRACE(effort_exclusion_flag) 
  f_mort.initialize();
  
  for (int f = fleet0; f <= nfleet;  f++)
  {
     if (effort_exclusion_flag == 0)
       f_mort(f) = q(f)*effort(f);

     else if (effort_exclusion_flag == 1)
     {
        if (f == 0)
           f_mort(f) = q(f);
        else
           f_mort(f) = q(f)*effort(f);
     }
     else if (effort_exclusion_flag == 2)
       f_mort(f) = q(f);
 
  } // for (int f = 1; f <= nfleet;  f++)

}
template void par_t<d3_array,dmatrix,dvector,double>::fish_mort_comp(d3_array& f_mort, const d3_array& effort);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::fish_mort_comp(dvar3_array& f_mort, const d3_array& effort);
