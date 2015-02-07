//$Id: dfexplike.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t.h"
#include "trace.h"


void getreturns(dmatrix& returns, const int fleet, const year_month& date, const int cohort, const recaptype_vector& recaps, const int nrec, const int m, const int n);

extern ofstream clogf;
extern int _global_report_flag;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_exp_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort)
{
  double tobs = 0.0;
  double tpred = 0.0;
  double dftpred = 0.0;
  //double dflatp = 0.0;
  //double dfa = 0.0;
  double dftemp = 0.0;
  int _m = param.get_m();
  int _n = param.get_n();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();

  dmatrix obs(1, _m, jlb, jub);
  dmatrix pred(1, _m, jlb, jub);
  dmatrix dfpred(1,_m, jlb, jub);
  dfpred.initialize();

  int nf = param.get_nfleet();
  for (int f = nf; f >= 1; f--)
  {
    if (effort_occured(f))
    {
      getreturns(obs, f, date, cohort, recaps, nrec, _m, _n);

      // recompute
      param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);


      for (int i=_m; i >=1; i--)
      {
        int lb = jlb(i);
        int ub = jub(i);
        for (int j=ub; j >= lb; j--)
        {
          // recompute
          tobs  = obs(i,j);
          tpred = pred(i,j);

          if (tpred > 0.0)
          {
            //like +=temp;
            dftemp += dflike;

            //temp = -(tobs/tpred + log(tpred));
            dftpred += dftemp*tobs/(tpred*tpred); // OK
            dftpred -= dftemp/tpred; //OK
            dftemp = 0.0;
          }
          //tpred = pred(i,j);
          dfpred(i,j) += dftpred;
          dftpred = 0.0;

        }  // j loop
      } // i  loop


    //param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);
      param.df_pred_recapture_comp(dfparam, pred, dfpred,
           pred_tags, dfpred_tags, z, dfz, fmort, dffmort, f, date);

    } //  if (effort_occured(f))
  } // fleet loop
}
template void df_exp_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<d3_array,dmatrix,dvector,double>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort);
template<> void df_exp_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort) {}
