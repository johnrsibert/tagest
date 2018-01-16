#include <fstream>
#include "precomp2.h"
#include "par_t.h"

#define TRACE(object) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #object " = " << object << endl;

extern ofstream clogf;

//void pred_recapture_comp(par_t& param, dmatrix& pred_recaptures,
//                         dmatrix& pred_tags, dmatrix& z, d3_array& fmort,
//                         int fleet)
void par_t::pred_recapture_comp(dmatrix& pred_recaptures,
                         dmatrix& pred_tags, dmatrix& z, d3_array& fmort, 
                         int fleet, year_month& date)
{
  if (m_ipar[27] == 0)
      pred_recaptures = elem_prod(fmort(fleet), pred_tags);
  else if (m_ipar[27] == 1)
  {
      dmatrix& f = fmort(fleet);
      dmatrix zz = -1.0 * z;
      dmatrix p = elem_prod(elem_div(f,z), (1.0-exp(zz)));
      pred_recaptures = elem_prod(p, pred_tags);
  }
  else
  {
      cerr << "Unknown option (" << m_ipar[27]
           << ") passed to pred_recapture_comp in " << __FILE__ << endl;
      exit(1);
  }

/*dmatrix tmp(1,param._get_m(), param.jlb, param.jub); tmp = pred_recaptures * param.get_report_rates(fleet); pred_recaptures = tmp;*/

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
        double tmp = pred_recaptures(ii,jj)/report_rates(ff);
        pred_recaptures(ii,jj) = tmp;

      }
    }
  }

}

#ifndef SIMULATE
//void df_pred_recapture_comp(par_t& param, par_t& dfparam,
//			dmatrix& pred_recaptures, dmatrix& dfpred_recaptures,
//			dmatrix& pred_tags, dmatrix& dfpred_tags,
//			dmatrix& z, dmatrix& dfz,
//			d3_array& fmort, d3_array& dffmort, int fleet)
void par_t::df_pred_recapture_comp(par_t& dfparam,
			dmatrix& pred_recaptures, dmatrix& dfpred_recaptures,
			dmatrix& pred_tags, dmatrix& dfpred_tags,
			dmatrix& z, dmatrix& dfz,
			d3_array& fmort, d3_array& dffmort, int fleet, year_month& date)
{


  // adjust anomalous fishing mortality events so r = 1.0;
  if (num_afm)
  {
    year_month afm_date;
    for (int i = num_afm; i >= 1; i--)
    {
      afm_date.set(afm_yr(i), afm_mo(i));
      int ff = afm_fl(i);
      if ( (afm_date == date)&& (ff == fleet) )
      {
        int ii = afm_i(i);
        int jj = afm_j(i);

        double dftmp = 0.0;

        //pred_recaptures(ii,jj) = report_rates(ff)/pred_recaptures(ii,jj);

        //pred_recaptures(ii,jj) = tmp;
        dftmp += dfpred_recaptures(ii,jj);
        dfpred_recaptures(ii,jj) = 0.0;

        //double tmp = pred_recaptures(ii,jj)/report_rates(ff);
        dfpred_recaptures(ii,jj) += dftmp/report_rates(ff); 
        dfparam.report_rates(ff)
                    -= dftmp*pred_recaptures(ii,jj)/(report_rates(ff)*report_rates(ff));
        dftmp = 0.0;

      }
    }
  }

  // pred_recaptures = pred_recaptures * param.report_rates[fleet];
  // tmp = pred_recaptures * param.report_rates[fleet];
  // pred_recaptures = tmp;

  dmatrix dftmp(1, m, jlb, jub);
  dftmp.initialize();

  // pred_recaptures = tmp;
  dftmp += dfpred_recaptures;
  dfpred_recaptures.initialize();


  // tmp = pred_recaptures * param.get_report_rates(fleet);
  //dfparam.report_rates[fleet] += sum( elem_prod(pred_recaptures,dftmp) );
  double temp_report_rates=dfparam.get_report_rates(fleet) + sum( elem_prod(pred_recaptures,dftmp) );
  dfparam.set_report_rates(fleet, temp_report_rates);
  dfpred_recaptures += report_rates(fleet)*dftmp;
  dftmp.initialize();

  // pred_recaptures = pred_recaptures * param.report_rates[fleet];
/*  dfparam.report_rates[fleet] += sum( elem_prod(pred_recaptures,dfpred_recaptures) ); dfpred_recaptures += param.report_rates[fleet]*dfpred_recaptures; dfpred_recaptures.initialize();*/

  if (m_ipar[27] == 0)
  {
    //TRACE(dfparam.ipar[27])
    //pred_recaptures = elem_prod(fmort(fleet), pred_tags);
    dfpred_tags += elem_prod(dfpred_recaptures, fmort(fleet));
    dffmort(fleet) += elem_prod(dfpred_recaptures, pred_tags);
    dfpred_recaptures.initialize();
  }
  else if (m_ipar[27] == 1)
  {
    //TRACE(dfparam.m_ipar[27])
    // recompute
    dmatrix& f = fmort(fleet);
    dmatrix expz = exp(-1.0*z);
    dmatrix expz1 = 1.0-expz;
    //dmatrix p = elem_prod(elem_div(f,z), (1.0-exp(-1.0*z)));
    //dmatrix p = elem_prod(elem_div(f,z), (1.0-expz));
    dmatrix p = elem_prod(elem_div(f,z), expz1);

    dmatrix& dff = dffmort(fleet);
    //dmatrix dfp(1, dfparam.m, 1, dfparam.n);
    //int m = dfparam.get_m();
    //ivector& jlb = dfparam.get_jlb();
    //ivector& jub = dfparam.get_jub();
    dmatrix dfp(1, m, jlb, jub);
    dfp.initialize();

    // derivate contributions
    //pred_recaptures = elem_prod(p, pred_tags);
    dfpred_tags += elem_prod(dfpred_recaptures, p);
    dfp += elem_prod(dfpred_recaptures, pred_tags);
    dfpred_recaptures.initialize();

    //dmatrix p = elem_prod(elem_div(f,z), (1.0-exp(-1.0*z)));
    //dff += elem_prod(elem_div((1.0-exp(-1.0*z)),z), dfp);
    dff += elem_prod(elem_div(expz1,z), dfp);
    /*
    dfz += elem_prod( elem_prod( elem_div(f,z),(exp(-1.0*z)-elem_div(1.0-exp(-1.0*z),z))),dfp);
    */
    dfz += elem_prod(
		 elem_prod(
		    elem_div(f,z), (expz-elem_div(expz1,z))
			   ),dfp);
    dfp.initialize();
  }
  else
  {
    cerr << "Unknown option (" << m_ipar[27]
	 << ") passed to df_pred_recapture_comp in " << __FILE__ << endl;
    exit(1);
  }
}
#endif
