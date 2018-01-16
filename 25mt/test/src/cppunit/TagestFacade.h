#ifndef __TagestFacade_h__
#define __TagestFacade_h__
/**
 * $Id: TagestFacade.h 2737 2010-08-05 22:14:06Z johnoel $
 */

#include <iostream>
#include <fstream>
#include <string>
using std::string;
#include <sstream>
using std::ostringstream;
#include <memory>
using std::auto_ptr;
//#include <log4cxx/logger.h>
//using namespace log4cxx;

#include <fvar.hpp>
#include <cifstrem.h>
#include "MinimizerTemplate.h"
//#include "log.h"

#include "par_t_nn.h"
#include "par_treg.h"
#include "coff_t.h"

void weight_penalty(neural_net_vector& nn, const double wts_weight, double& wts_penalty, const int m_ipar78);
void dfweight_penalty(neural_net_vector& nn, neural_net_vector& dfnn, const double wts_weight, double& dfwts_penalty, const int m_ipar78);

void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);
void dftotal_mort_comp(par_t& param, par_t& dfparam, d3_array& f_mort, d3_array& dff_mort, dmatrix& dfz);
int negative_tag_penalty(dmatrix& tags, par_t& param, double& pen);
void df_negative_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t& param, double& dfpen);
void total_tag_penalty(dmatrix& tags, par_t& param, double& pen);
void df_total_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t& param, double& dfpen);

void poisson_like(double& like, dmatrix& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t& param, dmatrix& z, ivector& effort_occured, d3_array& fmort);
void df_poisson_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t& param, par_t& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort);

void weight_switch_penalty(/*const*/ neural_net_vector& nn, const double switch_wt, double & wts_switch_penalty);
void dfweight_switch_penalty(neural_net_vector& nn, neural_net_vector& dfnn, const double switch_wt, double& dfsw_pen);

void negative_binomial_like(double& like, dmatrix& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t& param, dmatrix& z, ivector& effort_occured, d3_array& fmort, dvector& a, const int naa, const int nb_scale);
void df_nb_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t& param, par_t& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort, dvector& a, dvector& dfa, const int naa, const int nb_scale);

void exponential_like(double& like, dmatrix& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t& param, dmatrix& z, ivector& effort_occured, d3_array& fmort);
void df_exp_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t& param, par_t& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort);

void resid_comp(const dmatrix& obs, const dmatrix& pred, const dmatrix& like, const int report);
void uvs_freq_comp(const dmatrix& unode, const dmatrix& vnode, const dmatrix& snode, const imatrix& gridmap, const int report);


coff_t* create_coff_t(const Model_area& param, const int ipar6, const bool centered);
void readrecaps(par_t& param, const char* root, recaptype_vector& recaps, int& pnrecs, const char* path = DEFAULT_PATH);

double boundp(double x, double fmin, double fmax,double& fpen);
double dfboundp(double x, double fmin, double fmax, double& g);

void reverse(double& like, dvector& g, int nvar);

#include "intersav.h"
extern int saveinterflag;
extern setsaveinterflag interflag;
extern int _global_report_flag;
extern int saveinterflag;
extern intersavetype *isp;

class TagestFacade: public MinimizerTemplate
{
public:
TagestFacade() { init(); }
TagestFacade(const TagestFacade&);
virtual ~TagestFacade() {
	par_t_nn &param = (par_t_nn&)*_param;
	param.savefit(_likelihood, _g.indexmax(), max(fabs(_g)));
}

public:
void xinit(dvector& x);
double run(const dvector& x, dvector& g);

dmatrix& get_bias() const {
	par_t_nn &param = (par_t_nn&)*_param;
	return param.nn(1).bias;
}

//private:
protected:
auto_ptr<par_t> _param;

auto_ptr<coff_t> _coff;
auto_ptr<coff_t> _dfcoff;

int nrec;
auto_ptr<recaptype_vector> _recaps;
auto_ptr<indexed_regional_fishery_record> _irfr;

private:
d3_array effort;
d3_array fish_mort;
d3_array dffish_mort;

dmatrix u;
dmatrix v;
dmatrix sigma;
dmatrix dfu;
dmatrix dfv;
dmatrix dfsigma;

dmatrix tot_mort;
dmatrix dftot_mort;
dmatrix release;
dmatrix dfrelease;

//static LoggerPtr logger;

private:
void init();
/**
 * Factory Method for param
 */
void make_par_t(const adstring filename);
/**
 * Factory Method for coff
 */
void make_coff_t();
/**
 *
 */
void likelihoodComp(double &likelihood, par_t_nn &param, dmatrix &release, const int cohort, year_month &date, const int nrec, const int nfleet, ivector &effort_occured);
void dflikelihoodComp(double &dflikelihood, par_t_nn &param, par_t_nn&dfparam, dmatrix &release, dmatrix &dfrelease, const int cohort, year_month &date, const int nrec, const int nfleet, ivector &effort_occured);
void get_releases(const par_t_nn &param, dmatrix &release, const year_month &date, int &last_rn, const int ncohort);
};

#endif
