//$Id: nblike.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "precomp.h"
#include "trace.h"

void resid_comp(const dmatrix& obs, const dmatrix& pred, const dmatrix& like, const int report);
double mm_gammln(const double xx);
void getreturns(dmatrix& returns, int fleet, year_month& date, int cohort, recaptype_vector& recaps, int nrec, int m, int n);

extern ofstream clogf;
extern int _global_report_flag;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void negative_binomial_like(DOUBLE& like, dmatrix& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort, dvector& aa, const int naa)
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
  //clogf << date << " tag density field " << sum(pred_tags) << endl;
  for (int f = 1; f <= nf; f++)
  {
    //TRACE(like)
    //TTRACE(f,param.fleet_names(f))
    if (effort_occured(f))
    {
      //adstring fleet = param.fleet_names[f];

      getreturns(obs, f, date, cohort, recaps, nrec, _m, _n);
      //clogf << "  " << date << " observed recapture field " << sum(obs) << endl;

      param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);
      //clogf << "  " << date << " predicted recapture field " << sum(pred) << endl;
      //TRACE(pred)

      if (_global_report_flag)
      {
      //clogf << "pred: " << cohort << " " << date << " " << f << " " << sum(pred) << endl;
      //clogf << "tags: " << cohort << " " << date << " " << f << " " << sum(pred_tags) << endl;
        pred_tac += sum(pred);
        obs_tac += sum(obs);

      }

      double a = 0;
      if ( (naa == 2) &&
           ( (param.fleet_names[f] == "phdo") ||
             (param.fleet_names[f] == "iddo") ) )
        a = aa(2);
      else
        a = aa(1);

      int count = 0;
      temp = 0.0;
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
            count ++;
            double latp = log(tpred + a);

            temp += mm_gammln(tobs+a) - mm_gammln(tobs+1.0);
            temp += -a*latp + tobs*log(tpred) - tobs*latp;

          }
        }  // j loop
      } // i loop
      temp += count*(a*log(a) - mm_gammln(a));
      like +=temp; ///recaps.get_total_recaps();

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

} // end of negative_binomial_like
template void negative_binomial_like(double& like, dmatrix& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<d3_array,dmatrix,dvector,double>& param, dmatrix& z, ivector& effort_occured, d3_array& fmort, dvector& aa, const int naa);
template<> void negative_binomial_like(dvariable& like, dmatrix& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dvar_matrix& z, ivector& effort_occured, dvar3_array& fmort, dvector& aa, const int naa) {}
