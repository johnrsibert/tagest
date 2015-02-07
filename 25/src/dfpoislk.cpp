//$Id: dfpoislk.cpp 3066 2013-04-03 19:54:07Z jsibert $
#include "par_t.h"
#include "factoral.h"

extern factorial_class FACT;
extern double maximum_like_cont;

void getreturns(dmatrix& returns, const int fleet, const year_month& date, const int cohort, const recaptype_vector& recaps, const int nrec, const int m, const int n);
//void pred_recapture_comp(par_t& param, dmatrix& pred_recaptures, dmatrix& pred_tags,
//	      dmatrix& z, d3_array& fmort, int fleet);
//void df_pred_recapture_comp(par_t& param, par_t& dfparam,
//			dmatrix& pred_recaptures, dmatrix& dfpred_recaptures,
//			dmatrix& pred_tags, dmatrix& dfpred_tags,
//			dmatrix& z, dmatrix& dfz,
//			d3_array& fmort, d3_array& dffmort, int fleet);

//double mm_sfabs(double);
//double mm_dfsfabs(double);

//extern factorial_class FACT;
extern ofstream clogf;

//int factorial_class::instances = 0;
//int factorial_class::maxn = 0;
//dvector factorial_class::x;
//dvector factorial_class::lx;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_poisson_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort)
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
}
template void df_poisson_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<d3_array,dmatrix,dvector,double>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort);
template<> void df_poisson_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort) {}
