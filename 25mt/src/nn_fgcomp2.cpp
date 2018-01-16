//$Id: nn_fgcomp2.cpp 2842 2011-08-15 22:35:14Z jsibert $
#include "par_t_nn.h"
#include "intersav.h"
#include "fish_rec.h"
#include "trace.h"

#ifdef __GNUC__
  #if (__GNUC__ > 3)
    #include <iostream>
    #include <fstream>
    #include <iomanip>
    using namespace std;
  #else
    #include <iostream>
    #include <fstream.h>
    #include <iomanip.h>
  #endif
  #include <fcntl.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <unistd.h>
#endif

#ifdef __SUN__
#include <unistd.h>
#include <fcntl.h>
#endif


#if !defined(__SUN__) && !defined(__GNUC__)
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#endif

template <typename MATRIX1, typename MATRIX2>
void resid_comp(const MATRIX1& obs, const MATRIX2& pred, const MATRIX1& like, const int report);

template <typename MATRIX>
void uvs_freq_comp(const MATRIX& unode, const MATRIX& vnode, const MATRIX& snode, const imatrix& gridmap, const int report);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void total_tag_penalty(const MATRIX& tags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& pen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_total_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, double& dfpen);

template <typename MATRIX, typename DOUBLE>
void velpen(const MATRIX &u, const MATRIX &v, const double xweight, const double yweight, DOUBLE &penalty);
void dfvelpen(const dmatrix& u, const dmatrix& v, dmatrix& dfu, dmatrix& dfv, const double xweight, const double yweight, double& dfpenalty);

template <typename MATRIX, typename DOUBLE>
void sigpen(const MATRIX& s, const double weight, DOUBLE &penalty);
void dfsigpen(const dmatrix& s, dmatrix& dfs, const double weight, double& dfpenalty);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void weight_penalty(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, const double wts_weight, DOUBLE& wts_penalty, const int m_ipar78);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfweight_penalty(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn, const double wts_weight, double& dfwts_penalty, const int m_ipar78);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void hidden_wt_switch(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn,  double&  swpen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfhidden_wt_switch(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn, double & dfswpen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void weight_switch_penalty(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, const double switch_wt, DOUBLE& wts_switch_penalty);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfweight_switch_penalty(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn, const double switch_wt, double& dfsw_pen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int negative_tag_penalty(const MATRIX& tags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& pen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_negative_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, double& dfpen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void poisson_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_poisson_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_nb_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort, dvector& a, dvector& dfa, const int naa, const int nb_scale);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void negative_binomial_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort, VECTOR& a, const int naa, const int nb_scale);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void exponential_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_exp_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort);

d3_array make_d3_array(int sl,int sh, int nrl, int nrh, const ivector& ncl,
           const ivector& nch);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void clean(dvector& g, int nvar, par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& pdfpar, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& dfcoff,
       dmatrix& dfrelease,d3_array& dfz, double& plike);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void get_effort_array(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dftotal_mort_comp(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, d3_array& f_mort, d3_array& dff_mort, dmatrix& dfz);

//void df_fish_mort_comp(par_t& dfparam, d3_array& dff_mort, d3_array& effort);

template <typename DOUBLE>
void reverse(DOUBLE& like, dvector& g, const int nvar);

extern int _global_report_flag;     //Set in main(); 0 while in fmin loop else 1
extern setsaveinterflag interflag;
extern intersavetype * isp;
extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void compute(
MATRIX& release, year_month &date,
const int cohort, recaptype_vector& recaps, const int nrec, MATRIX& tot_mort,
ivector &effort_occured, D3_ARRAY& fish_mort, const int nfleet, DOUBLE& plike,
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE> &param) {
        DOUBLE plike1 = 0.0;
	const int optionPoisson = param.m_ipar[27];
        switch (param.m_ipar[11])
        {
          case 2:
            if (optionPoisson == 0 || optionPoisson == 1) {
               poisson_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort);
            } //end_if
            //
            //else if (param.ipar[27] == 2)
            //{
            //    poisson_like(plike1, psi_bar, date, cohort, recaps,
            //                 nrec, param, tot_mort, effort_occured,
            //                 fish_mort);
            //}
            else
            {
              cerr << "Error: Unknown option poisson m_ipar[27] = \"" << optionPoisson << "\" passed to fgcomp in " << __FILE__ << endl;
              exit(1);
            } //end_else
            break;

          case 3:
          //{
          //    least_square_like(plike1, release, date, cohort, recaps,
          //                      nrec, param, effort, fish_mort);
          //}
            cerr << "switch[11] = 3; "
                 << "least squares likelihood not implemented" << endl;
            exit(1);
            break;
          case 4:
          case 5:
            negative_binomial_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort,param.nb_par,1,param.m_ipar[18]);
            break;
          case 14:
          case 15:
            negative_binomial_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort,param.nb_par,2,param.m_ipar[18]);
            break;
          case 24:
          case 25:
            negative_binomial_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort,param.nb_par,nfleet,param.m_ipar[18]);
            break;
          case 6:
            exponential_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort);
            break;
          default:
            cerr << "Unknown likelihood function specified"
                 << "; param.m_ipar[11] = " << param.m_ipar[11]  << endl;
            exit(1);
        } // end switch


        plike += plike1;
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
double myfcomp(const year_month &local_start_date, const year_month &final_date, 
		const int pool_tags, int &kmonth, int &last_rn, const int ncohort, MATRIX& release,
		MATRIX& u, MATRIX& v, MATRIX& sigma,
		DOUBLE& plike, double &total_uv_penalty, double &total_sigma_penalty,
		coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE> &coff, indexed_regional_fishery_record& irfr, d3_array &effort, ivector &effort_occured, D3_ARRAY& fish_mort, MATRIX& tot_mort,
		const int cohort, recaptype_vector& recaps, const int nrec,
		const int nfleet,
		double &total_negative_tag_penalty,
		double &total_total_tag_penalty,
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE> &param) {
	coff.clean();

    //clogf << "---------- starting forward loop ----------" << endl;
    for (year_month date = local_start_date; date <= final_date; date++)
    {
       kmonth ++;
      if (pool_tags == 1)
      {
        while((last_rn < ncohort)
            && (date == param.get_tr_date(last_rn+1)))
        {
          last_rn++;
          const int i = param.get_tr_i(last_rn);
          const int j = param.get_tr_j(last_rn);
          release(i,j) += param.get_tr_tags(last_rn);
        }
      }
      param.uvs_comp(u, v, sigma, date);

      if (param.m_ipar[33] == 1) {
		const double uv_xweight = (const double)param.m_ipar[31]*1e-9;
		const double uv_yweight = (const double)param.m_ipar[32]*1e-9;
        DOUBLE uv_penalty = 0.0;
        velpen<MATRIX,DOUBLE>(u, v, uv_xweight, uv_yweight, uv_penalty);
        plike -= uv_penalty;
	param.add_total_uv_penalty(uv_penalty);
      }

      if(param.m_ipar[41] == 1) {
		const double sig_weight = (const double)param.m_ipar[42]*1e-9;
        DOUBLE sigma_penalty = 0.0;
        sigpen<MATRIX,DOUBLE>(sigma, sig_weight, sigma_penalty);
        plike -= sigma_penalty;
	param.add_total_sigma_penalty(sigma_penalty);
      }
	coff.comp(param, u, v, sigma);

      get_effort_array(param, irfr, effort, date, effort_occured);
      param.fish_mort_comp(fish_mort, effort, date);
      param.total_mort_comp(fish_mort, tot_mort);

	coff.b_adjust(param, tot_mort);

compute(release, date, cohort, recaps, nrec, tot_mort, effort_occured, fish_mort, nfleet, plike, param);

      DOUBLE neg_tag_pen = 0;
      negative_tag_penalty<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(release, param, neg_tag_pen);
	param.add_total_negative_tag_penalty(neg_tag_pen);
      plike -= neg_tag_pen;

      DOUBLE tot_tag_pen = 0;
      total_tag_penalty<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(release, param, tot_tag_pen);
	param.add_total_total_tag_penalty(tot_tag_pen);
      plike -= tot_tag_pen;
interflag.msavep(release, "release", isp);
	coff.adi(release);
    }
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfmyfcomp(
year_month& date,
year_month& final_date,
year_month& local_start_date,
const int pool_tags,
coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff,
coff_t<d3_array,dmatrix,dvector,double>& dfcoff,
double& dfplike,
double& dftot_tag_pen,
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param,
par_t_nn<d3_array,dmatrix,dvector,double>& dfparam,
double& dfneg_tag_pen,
const int cohort,
recaptype_vector& recaps,
const int nrec,
MATRIX& tot_mort,
dmatrix& dftot_mort,
d3_array& effort,
D3_ARRAY& fish_mort,
d3_array& dffish_mort,
ivector& effort_occured,
const int nfleet,
double& dfuv_penalty,
MATRIX& u,
MATRIX& v,
MATRIX& sigma,
dmatrix& df_u,
dmatrix& df_v,
dmatrix& df_sigma,
double& dfsigma_penalty,
indexed_regional_fishery_record& irfr,
int& kmonth,
MATRIX& release,
dmatrix& dfrelease
) 
{
    //clogf << "\n---------- starting reverse loop ----------" << endl;
    for (date = final_date; date >= local_start_date;)
    {
      interflag.mgetp(release, "release", isp);
      dfcoff.dfadi(coff, release, dfrelease);

      //plike -= tot_tag_pen;
      dftot_tag_pen -= dfplike;
      //total_tag_penalty(release, param, tot_tag_pen);
      df_total_tag_penalty<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(release, dfrelease, param, dftot_tag_pen);
      //double tot_tag_pen = 0;
      dftot_tag_pen = 0.0;

      //plike -= neg_tag_pen;
      dfneg_tag_pen -= dfplike;
      //negative_tag_penalty(release, *this, neg_tag_pen, negatives);
      df_negative_tag_penalty<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(release, dfrelease, param, dfneg_tag_pen);
      //double neg_tag_pen = 0;
      dfneg_tag_pen = 0.0;

        switch(param.m_ipar[11])
        {
          case 2:
            if (param.m_ipar[27] == 0 || param.m_ipar[27] == 1)
            {
              df_poisson_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort);
            } //end_if
            //
            //else if (param.ipar[27] == 2)
            //{
            //    df_poisson_like(dfplike, psi_bar, dfpsi_bar, date,
            //                    cohort, recaps, nrec, param, dfparam,
            //                    tot_mort, dftot_mort, effort_occured,
            //                    fish_mort, dffish_mort);
            //}
            break;
          //case 3:
            //df_least_square_like(release, date, cohort, recaps, nrec,
            //                     param, dfparam, dfrelease, dfplike,
            //                     effort, fish_mort, dffish_mort);
            //break;
          case 4:
          case 5:
            df_nb_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort, param.nb_par, dfparam.nb_par, 1,param.m_ipar[18]);
            break;
          case 14:
          case 15:
            df_nb_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort, param.nb_par, dfparam.nb_par, 2,param.m_ipar[18]);
            break;
          case 24:
          case 25:
            df_nb_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort, param.nb_par, dfparam.nb_par, nfleet,param.m_ipar[18]);
            break;
          case 6:
            df_exp_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, param, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort);
            break;
          default:
            cerr << "Unknown likelihood function specified"
                 <<  "; m_ipar[11] = " << param.m_ipar[11]  << endl
                 << "This can't happen!" << endl;
            exit(1);
        }

      //coff.b_adjust(*this, tot_mort);
      dfcoff.dfb_adjust(coff, param, dfparam, dftot_mort);

      //total_mort_comp(fish_mort, tot_mort);
      dftotal_mort_comp<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(param, dfparam, fish_mort, dffish_mort, dftot_mort);

      //fish_mort_comp(fish_mort, effort, date);
	param.df_fish_mort_comp(dfparam, dffish_mort, effort, date);

      if (param.m_ipar[33] == 1)
      {
  double uv_xweight = (double)param.m_ipar[31]*1e-9;
  double uv_yweight = (double)param.m_ipar[32]*1e-9;
        //plike -= uv_penalty;
        dfuv_penalty -= dfplike;
        //velpen(u, v, uv_xweight, uv_yweight, uv_penalty);
        dfvelpen(u, v, df_u, df_v, uv_xweight, uv_yweight, dfuv_penalty);
        //double uv_penalty = 0.0;
	     dfuv_penalty = 0.0;
      }

      //coff.comp(*this, date, isp, u, v, sigma);
      dfcoff.dfcomp(coff, param, dfparam, u, v, sigma, date, df_u, df_v, df_sigma, isp);

      if (param.m_ipar[41] == 1)
      {
  double sig_weight = (double)param.m_ipar[42]*1e-9;
        //plike -= sigma_penalty;
	   dfsigma_penalty -= dfplike;
        //sigpen(sigma, sig_weight, sigma_penalty);
	   dfsigpen(sigma, df_sigma, sig_weight, dfsigma_penalty);
        //double sigma_penalty = 0.0;
	   dfsigma_penalty = 0.0;
      }


      //uvs_comp(u, v, sigma, date);
      dfparam.dfuvs_comp(param, df_u, df_v, df_sigma, date);

        date--;
        // recompute variables needed for adjoint computations
        get_effort_array<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(param, irfr, effort, date, effort_occured);
        param.fish_mort_comp(fish_mort, effort, date);
        param.total_mort_comp(fish_mort, tot_mort);
        coff.b_adjust(param, tot_mort);


      kmonth --;
    } //End reverse month loop


    if (pool_tags == 2)
    {
      //cohort = get_tr_cohort(cc);
      //local_start_date = get_tr_date(cc);
      //final_date = local_start_date + nmonth;

      //release.initialize();
      dfrelease.initialize();

      //while((last_rn < ncohort)
      //            && (cohort == get_tr_cohort(last_rn+1)))
      //{
      //  last_rn++;
      //  i = get_tr_i(last_rn);
      //  j = get_tr_j(last_rn);
      //  release(i,j) += get_tr_tags(last_rn);
      //  clogf << local_start_date <<", released "
      //        << get_tr_tags(last_rn) << " at " << i << "," << j << endl;
      //}
    } //end_if
}
template 
void dfmyfcomp(
year_month& date,
year_month& final_date,
year_month& local_start_date,
const int pool_tags,
coff_t<d3_array,dmatrix,dvector,double>& coff,
coff_t<d3_array,dmatrix,dvector,double>& dfcoff,
double& dfplike,
double& dftot_tag_pen,
par_t_nn<d3_array,dmatrix,dvector,double>& param,
par_t_nn<d3_array,dmatrix,dvector,double>& dfparam,
double& dfneg_tag_pen,
const int cohort,
recaptype_vector& recaps,
const int nrec,
dmatrix& tot_mort,
dmatrix& dftot_mort,
d3_array& effort,
d3_array& fish_mort,
d3_array& dffish_mort,
ivector& effort_occured,
const int nfleet,
double& dfuv_penalty,
dmatrix& u,
dmatrix& v,
dmatrix& sigma,
dmatrix& df_u,
dmatrix& df_v,
dmatrix& df_sigma,
double& dfsigma_penalty,
indexed_regional_fishery_record& irfr,
int& kmonth,
dmatrix& release,
dmatrix& dfrelease
);
template <>
void dfmyfcomp(
year_month& date,
year_month& final_date,
year_month& local_start_date,
const int pool_tags,
coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& coff,
coff_t<d3_array,dmatrix,dvector,double>& dfcoff,
double& dfplike,
double& dftot_tag_pen,
par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param,
par_t_nn<d3_array,dmatrix,dvector,double>& dfparam,
double& dfneg_tag_pen,
const int cohort,
recaptype_vector& recaps,
const int nrec,
dvar_matrix& tot_mort,
dmatrix& dftot_mort,
d3_array& effort,
dvar3_array& fish_mort,
d3_array& dffish_mort,
ivector& effort_occured,
const int nfleet,
double& dfuv_penalty,
dvar_matrix& u,
dvar_matrix& v,
dvar_matrix& sigma,
dmatrix& df_u,
dmatrix& df_v,
dmatrix& df_sigma,
double& dfsigma_penalty,
indexed_regional_fishery_record& irfr,
int& kmonth,
dvar_matrix& release,
dmatrix& dfrelease
) {}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void mydfreset(
dvector& g,
VECTOR& x,
double& dfplike,
double& dftotal_wts_switch_penalty,
double& dftotal_wts_penalty,
par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param,
par_t_nn<d3_array,dmatrix,dvector,double>& dfparam
)
{
  if(param.m_ipar[81] == 1)
   {
  double switch_wt= (double) param.m_ipar[82]*1.0e-9;
       //plike -=  total_wts_switch_penalty;
        dftotal_wts_switch_penalty -= dfplike;
       //weight_switch_penalty(nn,switch_wt, total_wts_switch_penalty);
        dfweight_switch_penalty(param.nn, dfparam.nn, switch_wt, dftotal_wts_switch_penalty);
        //total_wts_switch_penalty = 0.0;
        dftotal_wts_switch_penalty = 0.0;
    }

  if(param.m_ipar[78] > 0)
  {
  double wts_weight =(double)param.m_ipar[79]*1.0e-9;
    //plike -= total_wts_penalty;
    dftotal_wts_penalty -= dfplike;
    //weight_penalty(nn, wts_weight, total_wts_penalty, m_ipar[78]);
    dfweight_penalty(param.nn, dfparam.nn, wts_weight, dftotal_wts_penalty, param.m_ipar[78]);
    //total_wts_penalty  = 0.0;
    dftotal_wts_penalty = 0.0;
  }
  dfparam.dfreset_diff_hwts(param.nn, dfparam.nn);
  dfparam.dfreset(g, x);
}
template
void mydfreset(
dvector& g,
dvector& x,
double& dfplike,
double& dftotal_wts_switch_penalty,
double& dftotal_wts_penalty,
par_t_nn<d3_array,dmatrix,dvector,double>& param,
par_t_nn<d3_array,dmatrix,dvector,double>& dfparam
);
template <>
void mydfreset(
dvector& g,
dvar_vector& x,
double& dfplike,
double& dftotal_wts_switch_penalty,
double& dftotal_wts_penalty,
par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param,
par_t_nn<d3_array,dmatrix,dvector,double>& dfparam
){}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::fgcomp2(DOUBLE& plike, VECTOR& x, dvector& g, int _nvar, recaptype_vector& recaps, int nrec, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr)
{
  int   i, j, cohort;
  //clogf.setf(ios::fixed);
  clock_t time1 = clock();

  par_t_nn<d3_array,dmatrix,dvector,double> dfparam(*this);

  imatrix jlbm(1,nfleet,1,m);
  imatrix jubm(1,nfleet,1,m);
  ivector ilbv(1,nfleet);
  ivector iubv(1,nfleet);
  for (int s = 1; s <= nfleet; s++)
  {
    ilbv(s) = 1;
    iubv(s) = m;
    //jlbm(s) = jlb;
    //jubm(s) = jub;
    for (int i = 1; i <= m; i++)
    {
      jlbm(s,i) = jlb(i);
      jubm(s,i) = jub(i);
    } //end_for
  } //end_for
  d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm); effort.initialize();
  D3_ARRAY fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm); fish_mort.initialize();
  d3_array dffish_mort(1, nfleet, ilbv, iubv, jlbm, jubm); dffish_mort.initialize();


  MATRIX tot_mort(1,m, jlb, jub);     tot_mort.initialize();
  dmatrix dftot_mort(1,m, jlb, jub); dftot_mort.initialize();
  //Tags
  MATRIX release(1,m, jlb, jub); release.initialize();
  dmatrix dfrelease(1, m, jlb, jub); dfrelease.initialize();


  ivector effort_occured(1, nfleet); effort_occured.initialize();

#if !defined(__SUN__) && !defined(__GNUC__)
  if ( ( isp->savefile = open(isp->savefilename, O_RDWR|O_CREAT|O_BINARY,S_IREAD|S_IWRITE) ) < 0)
#else
#if defined ( __SUN__) ||  defined ( __GNUC__)
  if ( ( isp->savefile = open(isp->savefilename, O_RDWR | O_CREAT | O_TRUNC , 0660) ) < 0)
  //if ( ( isp->savefile = open(isp->savefilename, O_RDWR | O_CREAT | O_TRUNC , 0777) ) < 0)
#else
  if ( ( isp->savefile = creat(isp->savefilename, O_RDWR) ) < 0)
#endif
#endif
  {
    cerr <<"Cannot open " << isp->savefilename <<" for output.\n";
    exit(1);
  }

  plike = 0.0;
  fpen = 0.0;
  total_uv_penalty = 0;
  total_sigma_penalty = 0;
  total_negative_tag_penalty = 0;
  total_total_tag_penalty = 0 ;
  total_bounds_penalty = 0;
  total_wts_penalty = 0.0;
  total_wts_switch_penalty = 0.0;

  //set usergrid, mort, q
  reset(x);
  reset_diff_hwts(nn); //put back the diff_hwts into the weights
  plike -= fpen;
	add_total_bounds_penalty(fpen);

  double wts_weight =(double)m_ipar[79]*1.0e-9;
  if(m_ipar[78] > 0)
  {
    total_wts_penalty  = 0.0;
    weight_penalty(nn, wts_weight, total_wts_penalty, m_ipar[78]);
    plike -= total_wts_penalty;
  }

  double switch_wt= (double) m_ipar[82]*1.0e-9;
  if(m_ipar[81]  == 1)
    {
      total_wts_switch_penalty = 0.0;
      weight_switch_penalty(nn,switch_wt, total_wts_switch_penalty);
      plike -=  total_wts_switch_penalty;
    }

  int ncohort = nrelease;

  int pool_tags = m_ipar[13];
  int omit_release_month = 0;
  if (m_ipar[14])
  {
    cerr << "Omit release month option (flag 14) not currently supported." << endl;
    exit(1);
  } //end_if
  int last_rn = 0;
  int c1 = 1;
  int c2 = ncohort;

  if (pool_tags == 1)
  {
    cohort = 0;
    c2 = 1;
  } //end_if
  else if (pool_tags == 2)
  {
    last_rn = 0;
    c1 = 1;
    c2 = ncohort;
  } //end_if

  //Use a macro for cleaner code
  ivector jlb1(0,m+1);
  ivector jub1(0,m+1);
  for (i = 1; i <= m; i++)
  {
    jlb1(i) = jlb(i) - 1;
    jub1(i) = jub(i) + 1;
  } //end_for
  jlb1(0) = 0;
  jlb1(m+1) = 0;
  jub1(0) = n+1;
  jub1(m+1) = n+1;

  MATRIX u(0, m+1, jlb1, jub1);          u.initialize();
  MATRIX v(0, m+1, jlb1, jub1);          v.initialize();
  MATRIX sigma(0, m+1, jlb1, jub1);   sigma.initialize();

  dmatrix df_u(0, m+1, jlb1, jub1); df_u.initialize();
  dmatrix df_v(0, m+1, jlb1, jub1); df_v.initialize();
  dmatrix df_sigma(0, m+1, jlb1, jub1); df_sigma.initialize();

  //dvector dfwts = get_active_weights_vector(); dfwts.initialize();

  year_month date;
  year_month local_start_date = start_date;
  year_month save_start_date = start_date;
  year_month final_date = get_tr_date(nrelease) + nmonth;

  double uv_xweight = (double)m_ipar[31]*1e-9;
  double uv_yweight = (double)m_ipar[32]*1e-9;
  double sig_weight = (double)m_ipar[42]*1e-9;
  double dfplike = 1.0;
  double dfneg_tag_pen = 0.0;
  double dftot_tag_pen = 0.0;
  double dfuv_penalty = 0.0;
  double dfsigma_penalty = 0.0;
  double dftotal_wts_penalty = 0.0;
  double dftotal_wts_switch_penalty = 0.0;

  dfcoff.clean();
  g.initialize();

  //funnel_dvariable kfl;
  for (int cc = c1; cc <= c2; cc++)
  {
    interflag.isp_reset();
    if (pool_tags == 2)
    {
      cohort = get_tr_cohort(cc);
      //clogf << "\nReleasing tags for cohort " << cohort << endl;
      local_start_date = get_tr_date(cc);
      final_date = local_start_date + nmonth;
      start_date = local_start_date;
      release.initialize();
      while( (last_rn < ncohort) &&
             (cohort == get_tr_cohort(last_rn+1)) )
      {
        last_rn++;
        i = get_tr_i(last_rn);
        j = get_tr_j(last_rn);
        release(i,j) += get_tr_tags(last_rn);
        //clogf << local_start_date <<", released "
        //      << get_tr_tags(last_rn) << " at " << i << "," << j << endl;
      } //end_while
      cc = last_rn;
    } //end_if

    int total_months = final_date - start_date + 1;
    int kmonth = 0;

	//ad_begin_funnel();
	//dvariable templike = 0;
    myfcomp(local_start_date, final_date,
		pool_tags, kmonth, last_rn, ncohort, release,
		u, v, sigma,	
		//templike, total_uv_penalty, total_sigma_penalty,
		plike, total_uv_penalty, total_sigma_penalty,
		coff, irfr, effort, effort_occured, fish_mort, tot_mort,
		cohort, recaps, nrec,
		nfleet,
		total_negative_tag_penalty,
		total_total_tag_penalty,
	*this);
       //kfl = templike;
       //plike += kfl;

    interflag.iclose(isp);
dfmyfcomp<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(
date,
final_date,
local_start_date,
pool_tags,
coff,
dfcoff,
dfplike,
dftot_tag_pen,
*this,
dfparam,
dfneg_tag_pen,
cohort,
recaps,
nrec,
tot_mort,
dftot_mort,
effort,
fish_mort,
dffish_mort,
effort_occured,
nfleet,
dfuv_penalty,
u,
v,
sigma,
df_u,
df_v,
df_sigma,
dfsigma_penalty,
irfr,
kmonth,
release,
dfrelease
);
  } //for (int cc = c1; cc <= c2; cc++)
  //End of cohort loop


  start_date = save_start_date;

mydfreset(
g,
x,
dfplike,
dftotal_wts_switch_penalty,
dftotal_wts_penalty,
*this,
dfparam
);
  reverse(plike, g, _nvar);
  close(isp->savefile);


  if (_global_report_flag)
  {
   //reset_diff_hwts(nn); //put diff_wts into the model_weights (not neccssary)
    //for(int s =1; s<= num_nn; s++) {
    //TRACE(nn(s).get_diff_hwts() )
    //  }
   resid_comp(release, release, release, 1);
   imatrix bogus(1,m,1,n); bogus.initialize();
   uvs_freq_comp(release, release,release, bogus,1); //variables are just dummy ones...! msa

  }

  TRACE(total_uv_penalty)
  TRACE(total_sigma_penalty)
  TRACE(total_negative_tag_penalty)
  TRACE(total_wts_switch_penalty)
  TRACE(total_wts_penalty)
  TRACE(total_bounds_penalty)

  time_t time_sec;
  time(&time_sec);
  clock_t time2 = clock();
  double elapsed_time = (double)(time2-time1)/CLOCKS_PER_SEC;
  clogf << "Objective function value = " << setprecision(8) << plike
        << " elapsed time = " << setprecision(2) << elapsed_time
        << " seconds." << setprecision(5) << endl;
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::fgcomp2(double& plike, dvector& x, dvector& g, int _nvar, recaptype_vector& recaps, int nrec, coff_t<d3_array,dmatrix,dvector,double>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr);
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::fgcomp2(dvariable& plike, dvar_vector& x, dvector& g, int _nvar, recaptype_vector& recaps, int nrec, coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr);
