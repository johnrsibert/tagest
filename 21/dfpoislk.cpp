#include "precomp.h"
#include "factoral.h"

extern double maximum_like_cont;

void getreturns(dmatrix& returns, int fleet, year_month& date, int cohort,
		recaptype_vector& recaps, int nrec, int m, int n);
//void pred_recapture_comp(par_t& param, dmatrix& pred_recaptures, dmatrix& pred_tags,
//	      dmatrix& z, d3_array& fmort, int fleet);
//void df_pred_recapture_comp(par_t& param, par_t& dfparam,
//			dmatrix& pred_recaptures, dmatrix& dfpred_recaptures,
//			dmatrix& pred_tags, dmatrix& dfpred_tags,
//			dmatrix& z, dmatrix& dfz,
//			d3_array& fmort, d3_array& dffmort, int fleet);

//double mm_sfabs(double);
//double mm_dfsfabs(double);

extern factorial_class FACT;
extern ofstream clogf;

void df_poisson_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags,
	     year_month& date, int cohort,
	     recaptype_vector& recaps, int nrec, par_t& param, par_t& dfparam,
	     dmatrix& z, dmatrix& dfz,
	     ivector& effort_occured, d3_array& fmort, d3_array& dffmort)
{
  double tpred = 0.0;
  double dftpred = 0.0;
  double dftemp = 0.0;

  double tobs = 0.0;

  int _m = dfparam.get_m();
  int _n = dfparam.get_n();
  ivector& jlb = dfparam.get_jlb();
  ivector& jub = dfparam.get_jub();

  dmatrix obs_caps(1,_m, jlb, jub);
  dmatrix pred_caps(1,_m, jlb, jub );
  dmatrix dfpred_caps(1,_m, jlb, jub);
  dfpred_caps.initialize();

  int nf = dfparam.get_nfleet();
  for (int f = nf; f >= 1; f--)
  {
    if (effort_occured(f))
    {
      //adstring fleet = dfparam.fleet_names[f];
      getreturns(obs_caps, f, date, cohort, recaps, nrec, _m, _n);

      // recompute
      param.pred_recapture_comp(pred_caps, pred_tags, z, fmort, f, date);

      for (int i=_m; i >= 1; i--)
      {
        int lb = jlb(i);
        int ub = jub(i);
        for (int j=ub; j >= lb; j--)
        {
          // recompute
          tobs  = obs_caps[i][j];
	        tpred = pred_caps[i][j];

          // derivative computations start here
          if (tpred > 0.0)
          {
            //recompute
            double temp = -(tobs*log(tpred) - tpred - FACT.log_factorial((int)tobs));
            if (temp < maximum_like_cont)
            {
              //like -= temp;
              dftemp -= dflike;

              // temp = -(tobs*log(tpred) - tpred - FACT.log_factorial((int)tobs));
              dftpred -= dftemp*(tobs/tpred-1);
              dftemp = 0.0;
            }
          }

          //tpred = pred_caps[i][j];
          dfpred_caps(i,j) += dftpred;
     	    dftpred = 0.0;

          //tobs  = obs_caps[i][j];
        } // j loop
      } // i loop

     // pred_recapture_comp(pred_caps, pred_tags, fmort, f);

     param.df_pred_recapture_comp(dfparam, pred_caps, dfpred_caps,
			                 pred_tags, dfpred_tags, z, dfz, fmort, dffmort, f, date);
    } // if (effort_occured(f))
    /*
    else
    {
      clogf << "df_poisson_like skipping fleet " << dfparam.fleet_names[f]
	     << " for " << date << endl;
    }
    */
  } // fleet loop
} // end of poisslk

