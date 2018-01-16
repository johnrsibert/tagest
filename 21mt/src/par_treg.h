#ifndef PAR_T_REG
#define PAR_T_REG
#include <fvar.hpp>
#include <adstring.hpp>
#include <iomanip>
//#include <jni.h>
#include "precomp2.h"
#include "par_t.h"
#include "fish_rec.h"
#include "intersav.h"
#include "linbcg.h"
#include "policy.h"
#include "recap_t.h"
#ifndef JNI_H
class JNIEnv;
class jobject;
#endif
typedef struct /* holds parameters specific to a region */
{
  double u;     // ew component positive to the east //
  double v;     // ns component positive to the north //
  double sigma; // dipsersion //
} gridpartype_;
declare(vector, gridpartype_);
declare(matrix, gridpartype_);

class gpm_ptr
{
  gridpartype_matrix* ptr;
public:
  friend class par_t_reg;
  gridpartype_vector& operator[](int i) { return (*ptr)[i];}
  gridpartype_vector& operator()(int i) { return (*ptr)[i];}
};

class par_t_reg : public par_t
{
private://Variables
  int      ngrid;          // number of usergrid regions
  int      nseason;        // number of seasons per year
  ivector  startmonth;     // vector of starting months for each season 
  ivector  seasons;        // season of each month
  gpm_ptr  usergrid;       // matrix of season/grid specific parameters
  imatrix  stratum_active; // switch active parameters by region/season strata
  //adstring par_file_version;

public://Functions
  par_t_reg();                   //par_treg.cpp
  par_t_reg(const par_t_reg& t); //par_treg.cpp
  par_t_reg(cifstream& prn);     //rpar.cpp-> rpreproc
  par_t_reg(const char * name);  //rparead.cpp-> rtagest
  ~par_t_reg();                  //par_treg.cpp

  void make_dfpar_t(par_t_reg& t);    //par_treg.cpp
  void dfparset(par_t_reg& dfparam);  //par_treg.cpp
  void read_seasons(cifstream& istr); //rparead.cpp
  void dfparkill();                   //par_treg.cpp" //


  //uvcomp.cpp
  void uvcomp(dmatrix& u, dmatrix& v, int season);
  //uvintrp3.cpp
  void uvinterpolate(dmatrix& u, dmatrix& v, year_month& ym);
  //sigcomp.cpp
  void sigma_comp(dmatrix& sigma, year_month& ym);
  //uvscomp.cpp
  void uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& ym);
  //dfuvs2.cpp
  void dfuvs_comp2(par_t_reg& param, par_t_reg& dfparam, dmatrix& dfu,
                  dmatrix& dfv, dmatrix& dfsigma, year_month& ym);

  //Virtual overwrites
  //nvarcalc.cpp
  int  nvarcalc();
  //xinit3.cpp
  void xinit(dvector& x, adstring_array& xname);
  //reset3.cpp
  void reset(dvector& x);
  //dfreset3.cpp
  void dfreset(dvector& g, dvector& x);
  //rfgcomp.cpp
  void fgcomp(double& plike, dvector& x, dvector& g, int nvar,
              recaptype_vector& recaps, int nrec,
              coff_t& coff, coff_t& dfcoff, indexed_regional_fishery_record& irfr);
  //rsavefit.cpp
  void savefit(const double like, const int nvar, const double gmax,
               const char* _ext = NULL);

  void simulate(JNIEnv* env, jobject jobj, recaptype_vector& recaps, int nrec,
                const adstring& path, const adstring& file);

  // john's additions for population simulator
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


private://Functions
  void allocate();         //par_treg.cpp
  void initialize();       //par_treg.cpp
  void set_seasons();      //par_seas.cpp

public://Access Functions
  void set_usergrid_u(int f, int s, double v){usergrid[f][s].u = v;}
  void set_usergrid_v(int f, int s, double v){usergrid[f][s].v = v;}
  void set_usergrid_sigma(int f, int s, double v){usergrid[f][s].sigma = v;}
  void set_nseason(int v) { nseason = v; }
  void set_ngrid(int v) { ngrid = v; }
  int get_season(const year_month& date);
  int get_ngrid() { return ngrid; }
  int get_nseason() { return nseason; }
  int get_startmonth(int s) { return startmonth[s]; }
  double get_usergrid_sigma(int f, int s) { return usergrid[f][s].sigma; }
  double get_usergrid_u(int f, int s) { return usergrid[f][s].u; }
  double get_usergrid_v(int f, int s) { return usergrid[f][s].v; }
  dvector get_seasonal_u(const int s);
  dvector get_seasonal_v(const int s);
  dvector get_seasonal_sigma(const int s);
  imatrix& get_stratum_active() { return stratum_active; }
  gridpartype_vector& get_usergrid(int s) { return usergrid[s]; }
};

ostream& operator << (ostream& s, par_t_reg& t);

#endif //#ifdef PAR_T_REG //
