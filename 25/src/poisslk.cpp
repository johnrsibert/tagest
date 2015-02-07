//$Id: poisslk.cpp 2878 2011-11-17 23:51:57Z jsibert $
#include "par_t.h"
#include "factoral.h"
#include "trace.h"

template < typename MATRIX1, typename MATRIX2 >
   void resid_comp(const MATRIX1 & obs, const MATRIX2 & pred,
		   const MATRIX1 & like, const int report);

void grief_map(dmatrix & x, par_t < d3_array, dmatrix, dvector,
	       double >&param, int iter, int ifn, int cohort,
	       year_month & date, char *fleet, double fval, int map,
	       int legend, int palette_size);

void getreturns(d3_array & returns, const year_month & date,
		const int cohort, const recaptype_vector & recaps,
		const int nrec);
void getreturns(dmatrix & returns, const int fleet,
		const year_month & date, const int cohort,
		const recaptype_vector & recaps, const int nrec,
		const int m, const int n);
//void pred_recapture_comp(par_t& param, dmatrix& pred_recaptures,
//                      dmatrix& pred_tags, dmatrix& z, d3_array& fmort, int fleet);

factorial_class FACT(100);
extern ofstream clogf;
extern int _global_report_flag;
double maximum_like_cont = 10000.0;



template < typename MATRIX > void sum(const MATRIX & m, double &v)
{
   v += sum(m);
}

template void sum(const dmatrix & m, double &v);
template <> void sum(const dvar_matrix & m, double &v)
{
   sum(value(m), v);
}

template < typename DOUBLE > void set(const DOUBLE v, double &d)
{
   d = v;
}

template void set(const double v, double &d);
template <> void set(const dvariable v, double &d)
{
   d = value(v);
}

// using d3_array for recaps
template < typename D3_ARRAY, typename MATRIX, typename VECTOR,
   typename DOUBLE > void poisson_like(DOUBLE & like, MATRIX & pred_tags,
				       year_month & date, int cohort,
				       d3_array & recaps, int nrec,
				       par_t < D3_ARRAY, MATRIX, VECTOR,
				       DOUBLE > &param, MATRIX & z,
				       ivector & effort_occured,
				       D3_ARRAY & fmort)
{
   double pred_tac = 0.0;
   double obs_tac = 0.0;
   DOUBLE tpred = 0.0;
   double tobs = 0.0;
   DOUBLE temp = 0.0;
   int _m = param.get_m();
   int _n = param.get_n();
   int nf = param.get_nfleet();
   ivector & jlb = param.get_jlb();
   ivector & jub = param.get_jub();

   MATRIX pred(1, _m, jlb, jub);
   pred.initialize();

   for (int f = 1; f <= nf; f++)
   {
      //if (effort_occured(f))
      {
	 dmatrix obs = recaps(f);
	 param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);
	 for (int i = 1; i <= _m; i++)
	 {
	    int lb = jlb(i);
	    int ub = jub(i);
	    for (int j = lb; j <= ub; j++)
	    {
	       tobs = obs(i, j);
	       tpred = pred(i, j);
               // Poisson liklihood is zero, if predicted recaptures is zero
	       if (tpred > 0.0)
	       {
                  // negtive log likelihood
		  temp = -(tobs * log(tpred) - tpred - FACT.log_factorial((int) tobs));
		  if (temp < maximum_like_cont)
		  {
		     like -= temp;
		  }
	       }
	    } // j loop
	 } // i loop
      } //  if (effort_occured(f))
   } // fleet loop


   /*
      if (_global_report_flag)
      {
      clogf << "\nlfreq: " << date << endl;
      for (int b = 1; b <= nlikebin; b++)
      {
      clogf << "lfreq: " << like_bins(b) << " " << like_count(b) << endl;
      }
      clogf << "tac: " << cohort << " " << date << " " << obs_tac << " " << pred_tac << endl;
      }
    */
}				// end of poisslk

template void poisson_like(double &like, dmatrix & pred_tags,
			   year_month & date, int cohort,
			   d3_array & recaps, int nrec, par_t < d3_array,
			   dmatrix, dvector, double >&param, dmatrix & z,
			   ivector & effort_occured, d3_array & fmort);
template void poisson_like(dvariable & like, dvar_matrix & pred_tags,
			   year_month & date, int cohort,
			   d3_array & recaps, int nrec,
			   par_t < dvar3_array, dvar_matrix, dvar_vector,
			   dvariable > &param, dvar_matrix & z,
			   ivector & effort_occured, dvar3_array & fmort);

// original version
template < typename D3_ARRAY, typename MATRIX, typename VECTOR,
   typename DOUBLE > void poisson_like(DOUBLE & like, MATRIX & pred_tags,
				       year_month & date, int cohort,
				       recaptype_vector & recaps, int nrec,
				       par_t < D3_ARRAY, MATRIX, VECTOR,
				       DOUBLE > &param, MATRIX & z,
				       ivector & effort_occured,
				       D3_ARRAY & fmort)
{
   double pred_tac = 0.0;
   double obs_tac = 0.0;
   DOUBLE tpred = 0.0;
   double tobs = 0.0;
   DOUBLE temp = 0.0;
   int _m = param.get_m();
   int _n = param.get_n();
   ivector & jlb = param.get_jlb();
   ivector & jub = param.get_jub();

   dmatrix obs(1, _m, jlb, jub);
   obs.initialize();
   MATRIX pred(1, _m, jlb, jub);
   pred.initialize();

   static int nlikebin = 22;
   static dvector like_bins;
   static dvector like_count;
   static dmatrix like_mat;
   if ((_global_report_flag) && (!like_bins))
   {
      like_mat.allocate(1, _m, jlb, jub);
      like_mat.initialize();
      like_bins.allocate(1, nlikebin);
      like_count.allocate(1, nlikebin);
      like_count.initialize();
      like_bins(1) = 0.015625;
      for (int b = 2; b <= nlikebin; b++)
	 like_bins(b) = like_bins(b - 1) * 2.0;
   }


   int nf = param.get_nfleet();
   //clogf << date << " tag density field " << sum(pred_tags) << endl;
   for (int f = 1; f <= nf; f++)
   {
      if (effort_occured(f))
      {
	 //adstring fleet = param.fleet_names[f];

	 getreturns(obs, f, date, cohort, recaps, nrec, _m, _n);
	 //clogf << "  " << date << " observed recapture field " << sum(obs) << endl;

	 param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);
	 //clogf << "  " << date << " predicted recapture field " << sum(pred) << endl;

	 if (_global_report_flag)
	 {
	    //clogf << "pred: " << cohort << " " << date << " " << f << " " << sum(pred) << endl;
	    //clogf << "tags: " << cohort << " " << date << " " << f << " " << sum(pred_tags) << endl;
	    //pred_tac += sum(pred);
	    sum < MATRIX > (pred, pred_tac);
	    obs_tac += sum(obs);

	 }

	 for (int i = 1; i <= _m; i++)
	 {
	    int lb = jlb(i);
	    int ub = jub(i);
	    for (int j = lb; j <= ub; j++)
	    {
	       tobs = obs[i][j];
	       tpred = pred(i, j);
	       if (tpred > 0.0)
	       {
		  temp =
		     -(tobs * log(tpred) - tpred -
		       FACT.log_factorial((int) tobs));
		  if (temp < maximum_like_cont)
		  {
		     like -= temp;
		  }
		  //else
		  if ((temp > 100.0) && (_global_report_flag))
		  {
		     clogf << "tobs = " << tobs << "; tpred = " << tpred
			<< ", temp = " << temp
			<< "  " << date << " " << param.fleet_names(f)
			<< "," << i << "," << j << endl;
		  }
		  if (_global_report_flag)
		  {
		     //like_mat(i,j) = temp;
		     set < DOUBLE > (temp, like_mat(i, j));
		     int b = 1;
		     for (; b < nlikebin; b++)
		     {
			if (temp < like_bins(b))
			{
			   break;
			}
		     }
		     like_count(b)++;
		  }

	       }
	    }			// j loop
	 }			// i loop

	 if (_global_report_flag)
	    resid_comp(obs, pred, like_mat, 0);
      }				//  if (effort_occured(f))

      /*
         else
         {
         clogf << "poisson_like skipping fleet " << param.fleet_names[f]
         << " for " << date << endl;
         }
       */
   }				// fleet loop


   if (_global_report_flag)
   {
      clogf << "\nlfreq: " << date << endl;
      for (int b = 1; b <= nlikebin; b++)
      {
	 clogf << "lfreq: " << like_bins(b) << " " << like_count(b) <<
	    endl;
      }
      clogf << "tac: " << cohort << " " << date << " " << obs_tac << " " <<
	 pred_tac << endl;
   }
}				// end of poisslk

template void poisson_like(double &like, dmatrix & pred_tags,
			   year_month & date, int cohort,
			   recaptype_vector & recaps, int nrec,
			   par_t < d3_array, dmatrix, dvector,
			   double >&param, dmatrix & z,
			   ivector & effort_occured, d3_array & fmort);
template void poisson_like(dvariable & like, dvar_matrix & pred_tags,
			   year_month & date, int cohort,
			   recaptype_vector & recaps, int nrec,
			   par_t < dvar3_array, dvar_matrix, dvar_vector,
			   dvariable > &param, dvar_matrix & z,
			   ivector & effort_occured, dvar3_array & fmort);
