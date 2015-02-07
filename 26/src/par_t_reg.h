//$Id: par_t_reg.h 3129 2014-02-03 22:10:56Z eunjung $
#ifndef __PAR_T_REG_H__
#define __PAR_T_REG_H__
//#include "coff_t.h"
#include <fvar.hpp>
#include <adstring.hpp>
#include <jni.h>
#include "modlarea.h"
#include "par_t.h"
//#include "fish_rec.h"
//#include "intersav.h"
//#include "linbcg.h"
//#include "policy.h"
#include "recap_t.h"

class par_t_reg:
  public par_t
{
 public:
   int ngrid;			///< number of usergrid regions
   int nseason;			///< number of seasons per year
   ivector startmonth;		///< vector of starting months for each season 
   ivector seasons;		///< season of each month
   dvar_matrix usergridU;		///< matrix of season/grid specific parameters
   dvar_matrix usergridV;		///< matrix of season/grid specific parameters
   dvar_matrix usergridSigma;	///< matrix of season/grid specific parameters
   imatrix stratum_active;	///< switch active parameters by region/season strata
   d3_array fadmap;           //fadmap matrices tree
   d3_array gx_fadmap;           //fadmap matrices tree
   d3_array gy_fadmap;           //fadmap matrices tree

 public:
   par_t_reg();			//par_treg.cpp
   par_t_reg(const par_t_reg &t);
   par_t_reg(cifstream & prn);	//rpar.cpp-> rpreproc
   par_t_reg(const char *name);	//rparead.cpp-> rtagest
   ~par_t_reg();		//par_treg.cpp

 public:
   dvariable fcomp(dvar_vector q, dvariable mort, dvariable special_mort,
		   dvar_vector report_rates, dvar_vector nb_par,
		   dvar_matrix seasonGridU, dvar_matrix seasonGridV,
		   dvar_matrix seasonGridSigma);
   dvariable addTotalUVPenalty(dvar_matrix & uSeasonGrid,
			       dvar_matrix & vSeasonGrid,
			       const int season);
   dvariable addTotalSigmaPenalty(dvar_matrix & sigmaSeasonGrid,
				  const int season);
   void FADGradComp(void);
   //void read_fadmap(dmatrix& fadmap, const int year, const int month);
 public:
   void make_dfpar_t(par_t_reg & t);	//par_treg.cpp
   void dfparset(par_t_reg & dfparam);	//par_treg.cpp
   void read_seasons(cifstream & istr);	//rparead.cpp
   void dfparkill();		//par_treg.cpp" //

   void uvcomp(dvar_matrix & u, dvar_matrix & v, const int season, const int t);
   void uvcomp(dvar_matrix & uSeasonGrid, dvar_matrix & vSeasonGrid,
	       dvar_matrix & u, dvar_matrix & v, const int season);

   void uvinterpolate(dvar_matrix & u, dvar_matrix & v, year_month & ym);
   void uvinterpolate(dvar_matrix & uSeasonGrid, dvar_matrix & vSeasonGrid,
		      dvar_matrix & u, dvar_matrix & v, const int season);

   void sigma_comp(dvar_matrix & sigma, year_month & ym);
   void sigma_comp(dvar_matrix & sigmaSeasonGrid, dvar_matrix & sigma,
		   const int season);

   void uvs_comp(dvar_matrix & u, dvar_matrix & v, dvar_matrix & sigma, year_month & ym);
   void uvs_comp(dvar_matrix & uSeasonGrid, dvar_matrix & vSeasonGrid,
		 dvar_matrix & sigmaSeasonGrid, dvar_matrix & u,
		 dvar_matrix & v, dvar_matrix & sigma, const int season);



   //rsavefit.cpp
   void savefit(const double like, const int nvar, const double gmax,
		const char *_ext = NULL);

   void simulate(JNIEnv * env, jobject jobj, recaptype_vector & recaps,
   		 int nrec, const adstring & path, const adstring & file);


   void reset_q(dvector & q_save, int &grow_time);
   void set_initial_q(dvector & q_save);
   void recruit_comp(dmatrix & recruitment, const dmatrix & pop,
		     const year_month & date, double &spawning_biomass);
   void population_initialize(dmatrix & pop, const double init_pop);

   // random walk simulators
   void walk(void);

   // halflife computation
   void halflife(indexed_regional_fishery_record & irfr);

   //bool isRegional() const { return true; }

   dvariable computeObjectiveFunctionValue(model_parameters & parameters);

 private:			//Functions
   void allocate();		//par_treg.cpp
   void initialize();		//par_treg.cpp
   void set_seasons();		//par_seas.cpp

 public:			//Access Functions
   void set_usergrid_u(const int f, const int s, dvariable v) { usergridU[f][s] = v; }
   void set_usergrid_v(const int f, const int s, dvariable v) { usergridV[f][s] = v; }
   void set_usergrid_sigma(const int f, const int s, dvariable v) { usergridSigma[f][s] = v; }
   void set_nseason(int v) { nseason = v; }
   void set_ngrid(int v) { ngrid = v; }
   int get_season(year_month & date);
   int get_ngrid() const { return ngrid; }
   int get_nseason() const { return nseason; }
   int get_startmonth(int s) { return startmonth[s]; }
   dvariable get_usergrid_sigma(int f, int s) const { return usergridSigma[f][s]; }
   dvariable get_usergrid_u(int f, int s) const { return usergridU[f][s]; }
   dvariable get_usergrid_v(int f, int s) const { return usergridV[f][s]; }
   dvar_vector get_seasonal_u(const int s);
   dvar_vector get_seasonal_v(const int s);
   dvar_vector get_seasonal_sigma(const int s);
   imatrix & get_stratum_active() { return stratum_active; }
   dvar_vector & get_usergridU(const int s) { return usergridU[s]; }
   dvar_vector & get_usergridV(const int s) { return usergridV[s]; }
   dvar_vector & get_usergridSigma(const int s) { return usergridSigma[s]; }
};

//ostream & operator <<(ostream & s, par_t_reg  &t);

#endif // #ifndef __PAR_T_REG_H__
