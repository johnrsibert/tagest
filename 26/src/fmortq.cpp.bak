//$Id: fmortq.cpp 2853 2011-09-21 19:15:11Z jsibert $
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
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::fish_mort_comp(D3_ARRAY& f_mort1, D3_ARRAY& f_mort0, const d3_array& effort, year_month& date, const MATRIX& tag_density, const d3_array& recaps0, const dvector& mean_effort)
{
  const int missing_effort_flag = m_ipar(53);
  const int effort_normalized_flag = m_ipar[52];
  const int baranov_flag = m_ipar[27];
  f_mort1.initialize();
  f_mort0.initialize();
  for (int f = 1; f <= nfleet;  f++)
  {
    f_mort1(f) = q(f)*effort(f);

    if (missing_effort_flag > 0)
    {
      const int i1 = f_mort1(f).rowmin();
      const int i2 = f_mort1(f).rowmax();
      for (int i = i1; i <= i2; i++)
      {
        const int j1 = f_mort1(f,i).indexmin();
        const int j2 = f_mort1(f,i).indexmax();
        for (int j = j1; j <= j2; j ++)
        {
          if (recaps0(f,i,j) > 0)
          {
            if (missing_effort_flag == 1)
            {
              if (baranov_flag == 0)
              {
                 f_mort0(f,i,j) = recaps0(f,i,j)/(report_rates(f)*tag_density(i,j) + 1e-8);
              }
              else if (baranov_flag == 1)
              {
                 //TTRACE(baranov_flag,effort_normalized_flag)
                 DOUBLE init_f0;
                 if (effort_normalized_flag == 1)
                 {
                   //TRACE(q(f))
                   init_f0  =  q(f);
                 }
                 else
                 {
                   //TTRACE(q(f),mean_effort(f))
                   init_f0 =  q(f)*mean_effort(f);
                 }
                 
                 //if (tag_density(i,j) <= 0.0)
                    //TTRACE(tag_density(i,j),date)
                 //TRACE(init_f0)
                 f_mort0(f,i,j) = NR_baranov_f0(recaps0(f,i,j), f_mort1(f,i,j), 
                                               tag_density(i,j),report_rates(f),
                                               init_f0);
              }
              if ( (f_mort0(f,i,j) >= 10.0) || (f_mort0(f,i,j) < 0.0) )
              {
                 //TTRACE(tag_density(i,j),date)
                 //TTRACE(f_mort0(f,i,j),recaps0(f,i,j))
              }
            }
            else if (missing_effort_flag == 2)
            {
              if (effort_normalized_flag == 1)
                f_mort0(f,i,j) =  q(f);
              else
                f_mort0(f,i,j) =  q(f)*mean_effort(f);
            }
          }
        }
      }
    }
  }
  if (num_afm)
  {
    year_month afm_date;
    for (int i = 1; i <= num_afm; i++)
    {
      afm_date.set(afm_yr(i), afm_mo(i));
      if (afm_date == date)
      {
        int ff = afm_fl(i);
        int ii = afm_i(i);
        int jj = afm_j(i);

        f_mort1(ff,ii,jj) = afm(i);

      }
    }
  }
}
template void par_t<d3_array,dmatrix,dvector,double>::fish_mort_comp(d3_array& f_mort, d3_array& f_mort0, const d3_array& effort, year_month& date, const dmatrix& tag_density, const d3_array& recaps0, const dvector& mean_effort);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::fish_mort_comp(dvar3_array& f_mort, dvar3_array& f_mort0, const d3_array& effort, year_month& date, const dvar_matrix& tag_density, const d3_array& recaps0, const dvector& mean_effort);


#ifndef SIMULATE
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::df_fish_mort_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, d3_array& dff_mort, d3_array& effort, year_month& dfdate)
{

  if (num_afm)
  {
    year_month afm_date;
    for (int i = num_afm; i >= 1; i--)
    {
      afm_date.set(afm_yr(i), afm_mo(i));
      if (afm_date == dfdate)
      {
        int ff = afm_fl(i);
        int ii = afm_i(i);
        int jj = afm_j(i);

        //f_mort(ff,ii,jj) = afm(i);
        dfparam.afm(i) += dff_mort(ff,ii,jj);
        dff_mort(ff,ii,jj) = 0.0;

      }
    }
  }

  for (int f = nfleet; f >=1;  f--)
  {
    //f_mort(f) = q(f)*effort(f);
    dfparam.q(f) += sum( elem_prod(effort(f), dff_mort(f)) );
    dff_mort(f).initialize();
  }
}
template void par_t<d3_array,dmatrix,dvector,double>::df_fish_mort_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, d3_array& dff_mort, d3_array& effort, year_month& dfdate);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::df_fish_mort_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, d3_array& dff_mort, d3_array& effort, year_month& dfdate);

/*
void df_fish_mort_comp(par_t& dfparam, d3_array& dff_mort, d3_array& effort)
{
  int nf = dfparam.get_nfleet();
  for (int f = nf; f >=1;  f--)
  {
    //f_mort(f) = param.get_q(f)*effort(f);
    //dfparam.q(f) += sum( elem_prod(effort(f), dff_mort(f)) );
    dfparam.set_q(f, dfparam.get_q(f) + sum( elem_prod(effort(f), dff_mort(f)) ) );
    dff_mort(f).initialize();
  }
}
*/
#endif
