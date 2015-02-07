#ifndef __PAR_T_NN__
#define  __PAR_T_NN__
#include <fvar.hpp>
#include <adstring.hpp>
#include <iomanip.h>
#include <jni.h>
//#include <values.h>
#include "precomp2.h"
#include "par_t.h"
#include "fish_rec.h"
#include "intersav.h"
#include "linbcg.h"
#include "policy.h"
#include "recap_t.h"
#include "neural_net.h"


#ifdef unix
  #define DEFAULT_PATH "../"
#else
  #define DEFAULT_PATH "..\\"
#endif

class par_t_nn : public par_t
{
protected: //Variables
  neural_net nn;        //neural_net object
  
  //variables for neural_net configuration
  int num_levels;             //number of layers of the neural_net
  ivector nodes_at_layer;     //nodes_at_layers
  
  //for error checking in uvs/dfuvs comp functions
  int nactive_input_layers;
  year_month wsc_start_date;

 //scaling function parameters to estimate in the model
  double u_slope;		//slope of u
  double u_offset;		//offset constant of u
  double v_slope;
  double v_offset;
  double sig_slope;
  double sig_offset;

  //scaling parameter ranges. not for estimation
  double min_weight;
  double max_weight;
  double min_bias;
  double max_bias;
  double min_slope;
  double max_slope;
  double min_offset;
  double max_offset;
  
 
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

public:// Functions, constructors
  par_t_nn(); 	            //null constructor
  par_t_nn(const par_t_nn& t);  //copy constructor
  par_t_nn(cifstream& prn);      //undefined
  par_t_nn(const char *name);   //nn_paread.cpp -> tagest14.cpp
  ~par_t_nn();

  //nn_inputlayer.cpp
  dvector get_input_layer(year_month& ym, const int lon, const int lat);
  
  //nn_uvscomp.cpp
  void uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& ym);

  //nn_dfuvscomp.cpp
  void dfuvs_comp(par_t_nn& param, dmatrix& dfu,
                  dmatrix& dfv, dmatrix& dfsigma, year_month& ym);

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

   // par_t_nn.cpp. to refresh wstress_curl with uistream at run time.   
   void get_wstress_curl(year_month& current_month, dmatrix& wstress_curl);
   

  //Virtual overwrites

  int  nvarcalc();	//nn_nvarcalc.cpp
  void xinit(dvector& x, adstring_array& xname);   //nn_xinit3.cpp
  void reset(dvector& x);	    //nn_reset3.cpp
  void dfreset(dvector& g, dvector& x);     //nn_dfreset3.cpp
 
  //nn_fgcomp.cpp
  void fgcomp(double& plike, dvector& x, dvector& g, int nvar,  recaptype_vector& recaps, int nrec,
           coff_t& coff, coff_t& dfcoff, indexed_regional_fishery_record& irfr);

  void savefit(const double like, const int nvar, const double gmax,
               const char* _ext = NULL); 	//nn_savefit.cpp


   //guts of rtagmove
  void simulate(JNIEnv* env, jobject jobj, recaptype_vector& recaps, int nrec,
                const adstring& path, const adstring& file);	  // nn_ fgsim10.cpp


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

private://Functions
  void allocate();
  void initialize();


};  // end of par_t_nn class definition

//scaling function for nn_output
double scale_fn(const double x, const double slope, const double offset,
                const double min, const double max);			// par_t_nn.cpp

 //complimentary derivative function for above
void dfscale_fn(const double x, const double slope, const double offset,
                  double& dfx, double& dfslope, double& dfoffset, double& dfy,
                  const double min, const double max);			//nn_dfuvs.comp.cpp

ostream& operator << (ostream& s, par_t_nn& t);  //not defined


 double   min(const dmatrix&  ); //par_t_nn.cpp; to find min of a dmatrix
 double max(const dmatrix& );  //par_t_nn.cpp  to find max of dmatrix


#endif // #ifndef __PAR_T_NN__
