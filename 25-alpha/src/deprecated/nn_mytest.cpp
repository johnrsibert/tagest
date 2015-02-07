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


  //Use a macro for cleaner code
  ivector jlb1(0,m+1);
  ivector jub1(0,m+1);
  for (int i = 1; i <= m; i++)
  {
    jlb1(i) = jlb(i) - 1;
    jub1(i) = jub(i) + 1;
  } //end_for
  jlb1(0) = 0;
  jlb1(m+1) = 0;
  jub1(0) = n+1;
  jub1(m+1) = n+1;

 // u  v and sigma  initializtion was commented out earlier.  msa  2/12/2002 8:10PM
  dmatrix u(0, m+1, jlb1, jub1);      u.initialize();
  dmatrix v(0, m+1, jlb1, jub1);      v.initialize();
  dmatrix sigma(0, m+1, jlb1, jub1);  sigma.initialize();

  dmatrix df_u(0, m+1, jlb1, jub1); df_u.initialize();
  dmatrix df_v(0, m+1, jlb1, jub1); df_v.initialize();
  dmatrix df_sigma(0, m+1, jlb1, jub1); df_sigma.initialize();



 double wts_weight =(double)m_ipar[79]*1e-9;
  if(m_ipar[78] > 0)
  {
    total_wts_penalty  = 0.0;
    weight_penalty(nn, wts_weight, total_wts_penalty, m_ipar[18]);
    plike -= total_wts_penalty;
   }

 double switch_wt=  (double)m_ipar[82] * 1e-3;
 if(m_ipar[81]  == 1)
   {
      HERE
      total_wts_switch_penalty = 0.0;
      weight_switch_penalty(nn,switch_wt, total_wts_switch_penalty);
      plike -=  total_wts_switch_penalty;
   }

  year_month date;
  year_month local_start_date = start_date;
  //year_month save_start_date = start_date;
  //year_month final_date = get_tr_date(nrelease) + nmonth;
   year_month final_date = start_date + 4;
   TTRACE(start_date, final_date)

  //TTRACE(local_start_date, final_date)

   /*
   for(int s = 1; s <= num_nn; s++)
   {
     ivector vnrow = nn(s).get_nrow();
     ivector vncol  = nn(s).get_ncol();
     if(nn(s).nn_ipar(5))
     {
       const d3_array weights = nn(s).get_weights();
       const int num_levels = nn(s).get_nlayer();
       for(int i = 1; i <  num_levels; i++)
        {
           for(int j  =1; j  <= vnrow(i); j++)
           {
             for(int k = 1; k <= vncol(i); k++)
             {
               plike -= weights(i,j,k)*weights(i,j,k); //get norm2 of weights
             } //for k
           } //for j
        } //for i
      } //if(s)
   } //for s
  */

   /* ----------forward loop-------------------*/
   for (date = local_start_date; date <= final_date; date++)
   {
        uvs_comp(u, v, sigma, date);   //calculate
        plike -=  norm2(u);
        plike -= norm2(v);
        plike -= norm2(sigma);

    } //year_month

   /* ----------reverse loop-------------------*/
   for(date = final_date;  date >= local_start_date; date --)
   {
      //recalculate
      uvs_comp(u, v, sigma, date);

       //plike -= norm2(u);
       df_u -= dfplike * 2.0 * u;
       //plike -= norm2(u);
       df_v -= dfplike * 2.0 * v;
       //plike -= norm2(sigma);
       df_sigma -= dfplike * 2.0 * sigma;

       //uvs_comp(dmU, dmV, dmSigma, date);
       dfparam.dfuvs_comp(*this, df_u, df_v, df_sigma, date );

    } //end of date loop

    //start_date = save_start_date;

    /*
    for(int s = num_nn; s >= 1; s--)
     {
       ivector vnrow = nn(s).get_nrow();
       ivector vncol  = nn(s).get_ncol();
       if(nn(s).nn_ipar(5))
       {
         const d3_array weights = nn(s).get_weights();
         d3_array dfweights = dfparam.nn(s).get_weights();
         const int num_levels = nn(s).get_nlayer();
         for(int i = num_levels-1; i >= 1; i--)
          {
             for(int j  =vnrow(i); j  >= 1; j--)
             {
               for(int k = vncol(i); k >= 1; k--)
               {
                  //plike += weights(i,j,k)*weights(i,j,k); //get norm2 of weights
                  dfweights(i,j,k) -= 2.0*weights(i,j,k)*dfplike;
               } //for k
             } //for j
          } //for i
        } //if(s)
     } //for s
  */


  if(m_ipar[81] == 1)
   {
       HERE
       //plike -=  total_wts_switch_penalty;
        dftotal_wts_switch_penalty -= dfplike;
       //weight_switch_penalty(nn,switch_wt, total_wts_switch_penalty);
       dfweight_switch_penalty(nn, dfparam.nn, switch_wt, dftotal_wts_switch_penalty);
       //total_wts_switch_penalty = 0.0;
       dftotal_wts_switch_penalty = 0.0;
    }


  if(m_ipar[78] > 0)
  {
    //plike -= total_wts_penalty;
    dftotal_wts_penalty -= dfplike;
    //weight_penalty(nn, wts_weight, total_wts_penalty, m_ipar[78]);
    dfweight_penalty(nn, dfparam.nn, wts_weight, dftotal_wts_penalty, m_ipar[78]);
    //total_wts_penalty  = 0.0;
    dftotal_wts_penalty = 0.0;
  }

  dfparam.dfreset(g, x);
  reverse(plike, g, _nvar);  // becuase of the negative sign!

  TRACE(total_wts_penalty)
  TRACE(total_wts_switch_penalty)
  TRACE(total_bounds_penalty)
  TRACE(norm2(u))
  TRACE(norm2(v))
  TRACE(norm2(sigma))
  clogf << "Objective function value = " << setprecision(8) << plike << endl;


} //end fgcomp.cpp
