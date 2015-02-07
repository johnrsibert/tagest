//$Id: par_t_nn.h 3125 2013-11-19 02:44:01Z jsibert $
#ifndef __PAR_T_NN__
#define __PAR_T_NN__
#include "coff_t.h"
#include <fvar.hpp>
#include <adstring.hpp>
#include <jni.h>
#include "par_t.h"
#include "fish_rec.h"
#include "intersav.h"
#include "linbcg.h"
#include "policy.h"
#include "recap_t.h"
#include "neural_net_vector.h"


void velpen(const dmatrix& u, const dmatrix& v, const double xweight, const double yweight, double& penalty);
void dfvelpen(const dmatrix& u, const dmatrix& v, dmatrix& dfu, dmatrix& dfv, const double xweight, const double yweight, double& dfpenalty);
void sigpen(const dmatrix& s, const double weight, double& penalty);
void dfsigpen(const dmatrix& s, dmatrix& dfs, const double weight, double& dfpenalty);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
class par_t_nn : public par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>
{
public:
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::sw_coord;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::nfleet;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::m;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::n;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::jlb;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::jub;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::fpen;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::total_uv_penalty;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::total_sigma_penalty;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::total_negative_tag_penalty;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::total_total_tag_penalty;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::total_bounds_penalty;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::nrelease;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::m_ipar;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::start_date;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::nmonth;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::nb_par;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_tr_date;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_tr_cohort;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_tr_i;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_tr_j;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_tr_tags;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::fish_mort_comp;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::total_mort_comp;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::mort;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::q;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::report_rates;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::afm;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::index_to_long;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::index_to_lat;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::num_afm;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_nfleet;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_file_version;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::sp;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::nvar;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfile_version;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::objective_function_value;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::maximum_gradient;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::pathname;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::minafm;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::maxafm;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::minmort;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::maxmort;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::minq;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::maxq;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::special_mort;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::minsig;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::maxsig;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::minr_rates;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::maxr_rates;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::minvel;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::maxvel;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::gridmap;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_gridmap;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_jlb;
using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_jub;

//protected: //Variables
public:
void test();
  neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE> nn;     //neural_net_vector object
  int num_nn;                       //number of neural_net_objects
  ivector num_levels;           //vector of the number of layers for each nn
  imatrix  nodes_at_layer;   //nodes_at_layers
  imatrix input_controls;      //for controlling nodes at input_layer


  //for error checking in uvs/dfuvs comp functions
  ivector  nactive_input_layers;


 //scaling function parameters to estimate in the model
DOUBLE u_slope;		//slope of u
DOUBLE u_offset;		//offset constant of u
DOUBLE v_slope;
DOUBLE v_offset;
DOUBLE sig_slope;
DOUBLE sig_offset;

DOUBLE uniform_sigma;//m_ipar[21] == 1 turns off sig_slope and sig_offset

  //scaling parameter ranges. not for estimation
  double min_weight;
  double max_weight;
  double min_bias;
  double max_bias;
  double min_slope;
  double max_slope;
  double min_offset;
  double max_offset;

  double min_diff_hwts;
  double max_diff_hwts;

  double xmin_sig, xmax_sig; //for use with scale_fn2
  double xmin_uv, xmax_uv; //for use with scale_fn2

  //neural_net input layers (native model data)
  dvector scaled_year;
  dvector scaled_month;
  dvector scaled_longitude;
  dvector scaled_latitude;

  //neural_net input_layer data (matrix form)
  dmatrix depth;        //dmatrix for depth data
  dmatrix numfad;       //dmatrix for number of fads;
  dmatrix elevation;    //dmatrix for elevatioin data
  dmatrix distance;     //dmatrix for distance data
  dmatrix wstress_ne;   //dmatrix for ne wind data
  dmatrix wstress_sw;   //dmatrix for sw wind data
  dmatrix wstress_curl; //dmatrix for wind_curls

  dmatrix soi_index;     //dmatrix for soi_index
  dmatrix forage;
  dmatrix sst;
  dmatrix zonal_current;
  dmatrix meridional_current;
  dmatrix habitat;

  //data file_extensions
  char* depth_file_ext;
  char* fads_file_ext;
  char* elev_file_ext;
  char* dist_file_ext ;
  char* wne_file_ext;
  char* wsw_file_ext;
  char* wsc_file_ext;
  char* soi_index_file_ext;
  char* forage_file_ext;
  char* sst_file_ext;
  char* zonal_current_file_ext;
  char* meridional_current_file_ext;
  char* habitat_file_ext;

DOUBLE total_wts_penalty;
DOUBLE total_wts_switch_penalty;

public:// Functions, constructors
  par_t_nn(); 	            //null constructor
  par_t_nn(const par_t_nn<d3_array,dmatrix,dvector,double>& t);
  par_t_nn(const par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>& t);
  par_t_nn(cifstream& prn);      //undefined
  par_t_nn(const char *name);   //nn_paread.cpp -> tagest14.cpp
  ~par_t_nn();

void velpen2(const dmatrix& u, const dmatrix& v, double& penalty) 
{
	const double xweight = (double)m_ipar[31]*1e-9;
	const double yweight = (double)m_ipar[32]*1e-9;
	velpen(u, v, xweight, yweight, penalty);
}
void dfvelpen2(const dmatrix& u, const dmatrix& v, dmatrix& dfu, dmatrix& dfv, double& dfpenalty)
{
	const double xweight = (double)m_ipar[31]*1e-9;
	const double yweight = (double)m_ipar[32]*1e-9;
	dfvelpen(u, v, dfu, dfv, xweight, yweight, dfpenalty);
}
void sigpen2(const dmatrix& s, double& penalty)
{
	const double weight = (double)m_ipar[42]*1e-9;
	sigpen(s, weight, penalty);
}
void dfsigpen2(const dmatrix& s, dmatrix& dfs, double& dfpenalty)
{
	const double weight = (double)m_ipar[42]*1e-9;
	dfsigpen(s, dfs, weight, dfpenalty);
}

  //nn_inputlayer.cpp
  dvector get_input_layer(const int s, year_month& ym, const int lon, const int lat);

  void uvs_comp(MATRIX& u, MATRIX& v, MATRIX& sigma, year_month& ym);

  //nn_dfuvscomp.cpp
  void dfuvs_comp(par_t_nn<d3_array,dmatrix,dvector,double>& param, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym);
  void dfuvs_comp(par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, year_month& ym);

  //nn_uvs_diagnose.cpp
  void uvs_diagnose(dmatrix& unode, dmatrix& vnode, dmatrix& snode,
  	  dmatrix& u, dmatrix& v, dmatrix& sigma, const char* = NULL);

  //par_t_nn_cpp  //reads and  scales the scaled_year vector!
  void read_initialize_scaled_year();

  //calculating nactive_input_layers
  void calc_nactive_layers(void);

   //par_t_nn.cpp reads  matrix-form data for nueral_net input
   void read_input_matrix_data(const char* FILE, const char* file_extension, dmatrix& t,
                                                  const char* path = DEFAULT_PATH );

   void habitat_data( const char* input_file_ext, year_month& current_month, dmatrix& hh);
   void allocate_input_data_file_extensions(void);

  //Virtual overwrites

  int  nvarcalc();	//nn_nvarcalc.cpp
  void xinit(dvector& x, adstring_array& xname);   //nn_xinit3.cpp
  //void reset(const VECTOR& x);	    //nn_reset3.cpp
  void dfreset(dvector& g, const dvector& x);     //nn_dfreset3.cpp

  //nn_fgcomp.cpp
  void fgcomp(double& plike, dvector& x, dvector& g, int nvar,  recaptype_vector& recaps, int nrec, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr);
  //void fgcomp2(DOUBLE& plike, VECTOR& x, dvector& g, int nvar,  recaptype_vector& recaps, int nrec, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr);

  void savefit(const double like, const int nvar, const double gmax,
               const char* _ext = NULL); 	//nn_savefit.cpp


   //guts of rtagmove
  void simulate(JNIEnv* env, jobject jobj, recaptype_vector& recaps, int nrec,
                const adstring& path, const adstring& file);	  // nn_ fgsim10.cpp

 void reset_diff_hwts(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn);   //nn_diff_hwts.cpp
 void dfreset_diff_hwts(neural_net_vector<d3_array,dmatrix,dvector,double>&nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn);
	void dfreset_diff_hwts(neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn) {
	}

  // John's additions for population simulator
  // not critical to implement soon
  void population_simulate(indexed_regional_fishery_record& irfr,
                            JNIEnv* env, jobject jobj);

  void population_simulate(policy& pol, indexed_regional_fishery_record& irfr,
                           JNIEnv *env, jobject jobj,
                           const adstring& path, const adstring& file);

  void reset_q(dvector& q_save, int& grow_time);
  void set_initial_q(dvector& q_save);
  void recruit_comp(dmatrix& recruitment, const dmatrix& pop,
                    const year_month& date, double& spawning_biomass);

  void population_initialize(dmatrix& pop, const double init_pop);

  // random walk simulators
  void walk(void);

  // halflife computation
  void halflife(indexed_regional_fishery_record& irfr);

bool isNeuralNet() const {
	return true;
} 

void reset(const dvar_vector& q,
	   const dvariable& mort,
	   const dvariable& special_mort,
	   const dvar_vector& report_rates,
	   const dvariable& u_slope,
	   const dvariable& u_offset,
	   const dvariable& v_slope,
	   const dvariable& v_offset,
	   const dvariable& sig_slope,
	   const dvariable& sig_offset,
	   const dvariable& uniform_sigma,
	   const dvar_vector& weights,
	   const dvar_vector& diff_hwts,
	   const dvar_vector& bias);

dvariable fcomp(dvar_vector q, dvariable mort, dvariable special_mort, dvar_vector report_rates, dvariable u_slope, dvariable u_offset, dvariable v_slope, dvariable v_offset, dvariable sigma_slope, dvariable sigma_offset, dvariable uniform_sigma, dvar_vector weights, dvar_vector diff_hwts, dvar_vector bias);

dvariable computeObjectiveFunctionValue(model_parameters& parameters);
void reset(const model_parameters& parameters, dvariable& plike);

private://Functions
  void allocate();
  void initialize();

	DOUBLE scalefn(const DOUBLE x, const DOUBLE slope, const DOUBLE offset, const double min, const double max)  const;
	void dfscalefn(const double x, double &dfx, double &dfy, const dvariable slope, const double dfslope, const dvariable offset, double &dfoffset, const double min, const double max) const {
		dfscalefn(x, dfx, dfy, value(slope), dfslope, offset, dfoffset, min, max);
	}
	void dfscalefn(const double x, double &dfx, double &dfy, const double slope, double &dfslope, const double offset, double &dfoffset, const double min, const double max) const;
};  // end of par_t_nn class definition

template <typename DOUBLE>
DOUBLE scale_fn(const DOUBLE x, const DOUBLE slope, const DOUBLE offset, const double min, const double max);


//linear scaling function
double scale_fn2(const double x, const double xmin, const double xmax, const double ymin, const double ymax);  //par_t_nn.cpp
//complimentary derivative function for above
void dfscale_fn2(const double x, const double xmin, const double xmax, const double ymin, const double ymax,
              double & dfx, double& dfy );   //nn_dfuvscomp.cpp

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
ostream& operator << (ostream& s, par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& t);  //not defined

double   min(const dmatrix&  ); //par_t_nn.cpp; to find min of a dmatrix
double   max(const dmatrix& );  //par_t_nn.cpp  to find max of dmatrix


#endif // #ifndef __PAR_T_NN__
