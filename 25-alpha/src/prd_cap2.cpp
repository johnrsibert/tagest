//$Id: prd_cap2.cpp 3206 2014-08-22 05:37:57Z jsibert $
#include "par_t.h"
#include "trace.h"

//void pred_recapture_comp(par_t& param, dmatrix& pred_recaptures,
//                         dmatrix& pred_tags, dmatrix& z, d3_array& fmort,
//                         int fleet)
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::pred_recapture_comp(MATRIX& pred_recaptures, const MATRIX& pred_tags, const MATRIX& z, const D3_ARRAY& fmort, const int fleet, const year_month& _date)
{
  ADUNCONST(year_month,date)
  if (m_ipar[27] == 0) // simple
      pred_recaptures = elem_prod(fmort(fleet), pred_tags);
  else if (m_ipar[27] == 1)  // Baranov
  {
      const MATRIX& f = fmort(fleet);
      MATRIX zz = -1.0 * z;
      MATRIX p = elem_prod(elem_div(f,z), (1.0-mfexp(zz)));
      pred_recaptures = elem_prod(p, pred_tags);
  }
  else
  {
      cerr << "Unknown option (" << m_ipar[27]
           << ") passed to pred_recapture_comp in " << __FILE__ << endl;
      exit(1);
  }


  pred_recaptures = pred_recaptures * report_rates(fleet);

  if (m_ipar(55) > 0)
  {
     const int dev_index = date - global_start_date;
     pred_recaptures *= mfexp(recapture_dev(dev_index));
  }

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
        DOUBLE tmp = pred_recaptures(ii,jj)/report_rates(ff);
        pred_recaptures(ii,jj) = tmp;

      }
    }
  }
}
template void par_t<d3_array,dmatrix,dvector,double>::pred_recapture_comp(dmatrix& pred_recaptures, const dmatrix& pred_tags, const dmatrix& z, const d3_array& fmort, const int fleet, const year_month& date);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::pred_recapture_comp(dvar_matrix& pred_recaptures, const dvar_matrix& pred_tags, const dvar_matrix& z, const dvar3_array& fmort, const int fleet, const year_month& date);

#ifndef SIMULATE
//void df_pred_recapture_comp(par_t& param, par_t& dfparam,
//			dmatrix& pred_recaptures, dmatrix& dfpred_recaptures,
//			dmatrix& pred_tags, dmatrix& dfpred_tags,
//			dmatrix& z, dmatrix& dfz,
//			d3_array& fmort, d3_array& dffmort, int fleet)
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::df_pred_recapture_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& pred_recaptures, dmatrix& dfpred_recaptures, dmatrix& pred_tags, dmatrix& dfpred_tags, dmatrix& z, dmatrix& dfz, d3_array& fmort, d3_array& dffmort, int fleet, year_month& date)
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
    dmatrix expz = mfexp(-1.0*z);
    dmatrix expz1 = 1.0-expz;
    //dmatrix p = elem_prod(elem_div(f,z), (1.0-mfexp(-1.0*z)));
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

    //dmatrix p = elem_prod(elem_div(f,z), (1.0-mfexp(-1.0*z)));
    //dff += elem_prod(elem_div((1.0-mfexp(-1.0*z)),z), dfp);
    dff += elem_prod(elem_div(expz1,z), dfp);
    /*
    dfz += elem_prod( elem_prod( elem_div(f,z),(mfexp(-1.0*z)-elem_div(1.0-mfexp(-1.0*z),z))),dfp);
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
template void par_t<d3_array,dmatrix,dvector,double>::df_pred_recapture_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& pred_recaptures, dmatrix& dfpred_recaptures, dmatrix& pred_tags, dmatrix& dfpred_tags, dmatrix& z, dmatrix& dfz, d3_array& fmort, d3_array& dffmort, int fleet, year_month& date);
template<> void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::df_pred_recapture_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& pred_recaptures, dmatrix& dfpred_recaptures, dmatrix& pred_tags, dmatrix& dfpred_tags, dmatrix& z, dmatrix& dfz, d3_array& fmort, d3_array& dffmort, int fleet, year_month& date) {}
#endif
