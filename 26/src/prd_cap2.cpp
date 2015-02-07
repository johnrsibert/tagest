//$Id: prd_cap2.cpp 2859 2011-10-03 18:52:55Z jsibert $
#include "par_t.h"

 
void par_t::pred_recapture_comp(dvar_matrix& pred_recaptures, const dvar_matrix& pred_tags, const dvar_matrix& z, const dvar3_array& fmort, const int fleet, const year_month& date)
{
  if (m_ipar[27] == 0)
      pred_recaptures = elem_prod(fmort(fleet), pred_tags);
  else if (m_ipar[27] == 1)
  {
      const dvar_matrix& f = fmort(fleet);
      dvar_matrix zz = -1.0 * z;
      dvar_matrix p = elem_prod(elem_div(f,z), (1.0-mfexp(zz)));
      pred_recaptures = elem_prod(p, pred_tags);
  }
  else
  {
      cerr << "Unknown option (" << m_ipar[27]
           << ") passed to pred_recapture_comp in " << __FILE__ << endl;
      exit(1);
  }


  pred_recaptures = pred_recaptures * report_rates(fleet);

  // adjust anomalous fishing mortality events so r = 1.0;
  if (num_afm)
  {
    year_month afm_date;
    for (int i = 1; i <= num_afm; i++)
    {
      afm_date.set(afm_yr(i), afm_mo(i));
      int ff = afm_fl(i);
      if ( (afm_date == date)&& (ff == fleet) )
      {
        int ii = afm_i(i);
        int jj = afm_j(i);

        //pred_recaptures(ii,jj) = report_rates(ff)/pred_recaptures(ii,jj);
        dvariable tmp = pred_recaptures(ii,jj)/report_rates(ff);
        pred_recaptures(ii,jj) = tmp;

      }
    }
  }
}
