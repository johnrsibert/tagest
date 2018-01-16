#include "precomp.h"
#include "trace.h"

double dfgammln(const double xx);

void getreturns(dmatrix& returns, int fleet, year_month& date, int cohort,
    recaptype_vector& recaps, int nrec, int m, int n);

extern ofstream clogf;
extern int _global_report_flag;
extern double min_tpred;

void df_nb_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags,
             year_month& date, int cohort,
             recaptype_vector& recaps, int nrec, par_t& param, par_t& dfparam,
             dmatrix& z, dmatrix& dfz,
             ivector& effort_occured, d3_array& fmort, d3_array& dffmort,
             dvector& aa, dvector& dfaa, const int naa, const int nb_scale)

{
  double tobs = 0.0;
  double tpred = 0.0;
  double dftpred = 0.0;
  double dflatp = 0.0;
  double dfa = 0.0;
  double dfw = 0.0;
  //double temp = 0.0;
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
  //clogf << date << " tag density field " << sum(pred_tags) << endl;
  for (int f = nf; f >= 1; f--)
  {
    if (effort_occured(f))
    {
      getreturns(obs, f, date, cohort, recaps, nrec, _m, _n);

      // recompute
      param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);
      /*
      int count = 0;
      int i;
      for (i=1; i <=_m; i++)
      {
        int lb = jlb(i);
        int ub = jub(i);
        for (int j=lb; j <= ub; j++)
        {
          tpred = pred(i,j);
          if (tpred > 0.0)
          {
            count ++;
          }
        }
      }
      */

      double w = 0;
      if (naa == nf)
        w = aa(f);
      else if ( (naa == 2) &&
           ( (param.fleet_names[f] == "phdo") ||
             (param.fleet_names[f] == "iddo") ) )
        w = aa(2);
      else
        w = aa(1);

      //like +=temp; ///recaps.get_total_recaps();
      dftemp += dflike; ///recaps.get_total_recaps();

      /*
      //temp += count*(a*log(a) - mm_gammln(a));
      dfa += count*dftemp*(1.0 + log(a));
      dfa -= count*dftemp*dfgammln(a);
      */

      for (int i=_m; i >=1; i--)
      {
        int lb = jlb(i);
        int ub = jub(i);
        for (int j=ub; j >= lb; j--)
        {
          // recompute
          tobs  = obs(i,j);
          tpred = pred(i,j);
          if (tpred > min_tpred) //0.0)
          {
            double a = 0;
            if (nb_scale)
              a = w*tpred;
            else
              a = w;
 
            double latp = log(tpred + a);

            // derivative computations start here

            //temp += a*(log(a) - latp) + tobs*(log(tpred)-latp);
            dfa += dftemp*(1.0 + log(a));
            dfa -= dftemp*latp;
            dflatp -= dftemp*a;
            dftpred += dftemp*tobs/tpred;
            dflatp -= dftemp*tobs;

             //temp += mm_gammln(tobs+a) - mm_gammln(a) - mm_gammln(tobs+1.0);
             dfa += dftemp*dfgammln(tobs+a);
             dfa -= dftemp*dfgammln(a);

            //double latp = log(tpred + a); // OK
            double rtpa = 1.0/(tpred+a);
            dftpred += dflatp*rtpa; ///(tpred+a);
            dfa += dflatp*rtpa; ///(tpred+a);
            dflatp = 0.0;


            if (nb_scale)
            {
              //a = w*tpred;
              dfw += dfa*tpred;
              dftpred += dfa*w;
              dfa = 0.0;
            }
            else
            {
              //a = w;
              dfw += dfa;
              dfa = 0.0;
            }
            //double a = 0;

          }
          //tpred = pred(i,j);
          dfpred(i,j) += dftpred;
          dftpred = 0.0;

        }  // j loop
      } // i  loop
      //temp = 0.0;
      dftemp = 0.0;

      if (naa == nf)
      {
        //w = aa(f);
        dfaa(f) += dfw;
        dfw = 0.0;
      }
      else if ( (naa == 2) &&
         ( (param.fleet_names[f] == "phdo") ||
           (param.fleet_names[f] == "iddo") ) )
      {
        //w = aa(2);
        dfaa(2) += dfw;
        dfw = 0.0;
      }
      else
      {
        //w = aa(1);
        dfaa(1) += dfw;
        dfw = 0.0;
      }

    //param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);
      param.df_pred_recapture_comp(dfparam, pred, dfpred,
           pred_tags, dfpred_tags, z, dfz, fmort, dffmort, f, date);

    } //  if (effort_occured(f))
  } // fleet loop
} // end of negative_binomial_like
