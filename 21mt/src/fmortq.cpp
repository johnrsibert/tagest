#include "par_t.h"

void par_t::fish_mort_comp(d3_array& f_mort, const d3_array& effort, year_month& date)
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

#ifndef SIMULATE
void par_t::df_fish_mort_comp(par_t& dfparam, d3_array& dff_mort,
                              d3_array& effort, year_month& dfdate)
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
