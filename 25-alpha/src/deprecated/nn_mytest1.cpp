#include "par_t_nn.h"
#include "trace.h"

void reverse(double& like, dvector& g, int nvar);
void weight_penalty(neural_net_vector& nn,
                    const double wts_weight, double& wts_penalty,
                    const int m_ipar78);
void dfweight_penalty(neural_net_vector& nn, neural_net_vector& dfnn,
                    const double wts_weight, double& dfwts_penalty,
                    const int m_ipar78);


void weight_switch_penalty(/*const*/ neural_net_vector& nn, const double switch_wt, double & wts_switch_penalty);
void dfweight_switch_penalty(neural_net_vector& nn, neural_net_vector& dfnn,
                      const double switch_wt, double& dfsw_pen);


extern _global_report_flag;

void par_t_nn::fgcomp(double& plike, dvector& x, dvector& g, int _nvar,
       recaptype_vector& recaps, int nrec,
       coff_t& coff, coff_t& dfcoff,
       indexed_regional_fishery_record& irfr)
{
  par_t_nn dfparam(*this);  //notice this is called before reset! i.e. have the values changed by minimizer
  plike = 0.0;
  fpen = 0.0;
  total_bounds_penalty = 0.0;
  total_wts_penalty = 0.0;
  total_wts_switch_penalty = 0.0;

  reset(x);
  plike -= fpen;

  total_bounds_penalty += fpen;

  double dfplike = 1.0;
  double dftotal_wts_penalty = 0.0;
  double dftotal_wts_switch_penalty = 0.0;

  g.initialize();



 // u  v and sigma  initializtion was commented out earlier.  msa  2/12/2002 8:10PM
  dmatrix u(1,m,1,n);      u.initialize();
  dmatrix v(1,m,1,n);      v.initialize();
  dmatrix sigma(1,m,1,n);  sigma.initialize();


  year_month date;
  year_month local_start_date = start_date;
  //year_month save_start_date = start_date;
  //year_month final_date = get_tr_date(nrelease) + nmonth;
   year_month final_date = start_date + 4;
   TTRACE(start_date, final_date)

    _global_report_flag = 0;

   TTRACE(local_start_date, final_date)
   /* ----------forward loop-------------------*/
   for (date = local_start_date; date <= final_date; date++)
   { 
        uvs_comp(u, v, sigma, date);   //calculate

    } //year_month

   HALT


} //end fgcomp.cpp
