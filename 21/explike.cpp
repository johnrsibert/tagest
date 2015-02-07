#include"precomp.h"

#include "trace.h"

void resid_comp(const dmatrix& obs, const dmatrix& pred, const dmatrix& like,
                const int report);
void getreturns(dmatrix& returns, int fleet, year_month& date, int cohort,
    recaptype_vector& recaps, int nrec, int m, int n);

extern ofstream clogf;
extern int _global_report_flag;

void exponential_like(double& like, dmatrix& pred_tags, year_month& date, int cohort,
       recaptype_vector& recaps, int nrec, par_t& param, dmatrix& z,
       ivector& effort_occured, d3_array& fmort)
{
  double pred_tac = 0.0;
  double obs_tac = 0.0;
  double tpred = 0.0;
  double tobs = 0.0;
  double temp = 0.0;
  int _m = param.get_m();
  int _n = param.get_n();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();

  dmatrix obs(1, _m, jlb, jub);
  dmatrix pred(1, _m, jlb, jub);

  static int nlikebin = 22;
  static dvector like_bins;
  static dvector like_count;
  static dmatrix like_mat;
  if ( (_global_report_flag) && (!like_bins) )
  {
     like_mat.allocate(1, _m, jlb, jub); 
     like_mat.initialize();
     like_bins.allocate(1,nlikebin);
     like_count.allocate(1,nlikebin);
     like_count.initialize();
     like_bins(1) = 0.015625;
     for (int b = 2; b <= nlikebin; b++)
       like_bins(b) = like_bins(b-1)*2.0;
  }


  int nf = param.get_nfleet();
  for (int f = 1; f <= nf; f++)
  {
    if (effort_occured(f))
    {
      getreturns(obs, f, date, cohort, recaps, nrec, _m, _n);
      param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);

      if (_global_report_flag)
      {
        pred_tac += sum(pred);
        obs_tac += sum(obs);

      }

      for (int i=1; i <=_m; i++)
      {
        int lb = jlb(i);
        int ub = jub(i);
        for (int j=lb; j <= ub; j++)
        {
          tobs  = obs(i,j);
          tpred = pred(i,j);
          if (tpred > 0.0)
          {
            temp = -(tobs/tpred + log(tpred));
            like +=temp;
          }
        }  // j loop
      } // i loop

      if (_global_report_flag)
        resid_comp(obs, pred, like_mat,0);
    } //  if (effort_occured(f))
  } // fleet loop


  if (_global_report_flag)
  {
     clogf << "\nlfreq: " << date << endl;
     for (int b = 1; b <= nlikebin; b++)
     {
       clogf << "lfreq: " << like_bins(b) << " " << like_count(b) << endl;
     }
    //clogf << "tac: " << cohort << " " << date << " " << obs_tac << " " << pred_tac << endl;
  }

} // end of exponential_like
