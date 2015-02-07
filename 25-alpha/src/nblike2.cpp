//$Id: nblike2.cpp 3189 2014-06-17 02:13:02Z jsibert $
#include "par_t.h"
#include "trace.h"

extern double min_nb_par;
extern double max_nb_par;

template <typename MATRIX1, typename MATRIX2>
void resid_comp(const MATRIX1& obs, const MATRIX2& pred, const MATRIX1& like, const int report);
template <typename DOUBLE>
void set(const DOUBLE v, double& d);
template <typename DOUBLE>
DOUBLE mm_gammln(const DOUBLE xx);

void getreturns(dmatrix& returns, const int fleet, const year_month& date, const int cohort, const recaptype_vector& recaps, const int nrec, const int m, const int n);

extern ofstream clogf;
extern int _global_report_flag;
double min_tpred = 1e-7;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void negative_binomial_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, d3_array& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort, VECTOR& aa, const int naa, const int nb_scale)
{
  double pred_tac = 0.0;
  double obs_tac = 0.0;
  DOUBLE tpred = 0.0;
  double tobs = 0.0;
  DOUBLE temp = 0.0;
  int _m = param.get_m();
  int _n = param.get_n();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();

  dmatrix obs(1, _m, jlb, jub); obs.initialize();
  MATRIX pred(1, _m, jlb, jub); pred.initialize();

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
      //getreturns(obs, f, date, cohort, recaps, nrec, _m, _n);
      dmatrix obs = recaps(f);

      param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);

      if (_global_report_flag)
      {
      //clogf << "pred: " << cohort << " " << date << " " << f << " " << sum(pred) << endl;
      //clogf << "tags: " << cohort << " " << date << " " << f << " " << sum(pred_tags) << endl;
        set(sum(pred), pred_tac);
        obs_tac += sum(obs);

      }

      DOUBLE w = 0;
      if (naa == nf)
        w = aa(f);
      else if ( (naa == 2) &&
           ( (param.fleet_names[f] == "phdo") ||
             (param.fleet_names[f] == "iddo") ) )
        w = aa(2);
      else
        w = aa(1);

      //int count = 0;
      temp = 0.0;
      for (int i=1; i <=_m; i++)
      {
        int lb = jlb(i);
        int ub = jub(i);
        for (int j=lb; j <= ub; j++)
        {
          tobs  = obs(i,j);

          tpred = pred(i,j);
          if (tpred > min_tpred) //0.0)
          {
            //count ++;
            DOUBLE a = 0;
            if (nb_scale)
              a = w*tpred;
            else
              a = w;
            DOUBLE latp = log(tpred + a);

            //temp += mm_gammln(tobs+a) - mm_gammln(tobs+1.0);
            //temp += -a*latp + tobs*log(tpred) - tobs*latp;
	    DOUBLE tobsa = tobs + a;
            temp += mm_gammln(tobsa) - mm_gammln(a) - mm_gammln(tobs+1.0);
            temp += a*(log(a) - latp) + tobs*(log(tpred)-latp);
          }
        }  // j loop
      } // i loop
      //temp += count*(a*log(a) - mm_gammln(a));
      like +=temp;

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
template void negative_binomial_like(double& like, dmatrix& pred_tags, year_month& date, int cohort, d3_array& recaps, int nrec, par_t<d3_array,dmatrix,dvector,double>& param, dmatrix& z, ivector& effort_occured, d3_array& fmort, dvector& aa, const int naa, const int nb_scale);
template void negative_binomial_like(dvariable& like, dvar_matrix& pred_tags, year_month& date, int cohort, d3_array& recaps, int nrec, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dvar_matrix& z, ivector& effort_occured, dvar3_array& fmort, dvar_vector& aa, const int naa, const int nb_scale);
