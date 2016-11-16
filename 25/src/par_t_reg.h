//$Id: par_t_reg.h 3129 2014-02-03 22:10:56Z eunjung $
#ifndef __PAR_T_REG_H__
#define __PAR_T_REG_H__
#include "coff_t.h"
#include <fvar.hpp>
#include <adstring.hpp>
#include <jni.h>
#include "modlarea.h"
#include "par_t.h"
#include "fish_rec.h"
#include "intersav.h"
#include "linbcg.h"
#include "policy.h"
#include "recap_t.h"

template < typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE > class par_t_reg:
  public par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >
{
 public:
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::nfleet;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::m;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::n;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::jlb;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::jub;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::fpen;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::total_uv_penalty;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::total_sigma_penalty;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::total_negative_tag_penalty;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::total_total_tag_penalty;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::total_bounds_penalty;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::nrelease;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::m_ipar;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::start_date;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::nmonth;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::nb_par;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_tr_date;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_tr_cohort;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_tr_i;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_tr_j;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_tr_tags;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::fish_mort_comp;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::total_mort_comp;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::mort;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::q;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::report_rates;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::afm;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::index_to_long;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::index_to_lat;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::num_afm;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_nfleet;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::par_file_version;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::sp;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::nvar;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::dfile_version;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::objective_function_value;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::maximum_gradient;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::pathname;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::minafm;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::maxafm;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::minmort;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::maxmort;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::minq;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::maxq;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::special_mort;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::minsig;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::maxsig;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::minr_rates;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::maxr_rates;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::minvel;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::maxvel;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::gridmap;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_gridmap;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_jlb;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::get_jub;
   //using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::sus_spd;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::chi;
   //using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::mean_size;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::min_D;
   //using par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::max_D;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::logslope;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::inflection;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::loggamma;
   //using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::fadmap;
   //using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::gx_fadmap;
   //using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::gy_fadmap;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::max_gx;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::max_gy;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::max_g;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::max_fad;
   //using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::set_sus_spd;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::set_chi;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::set_min_D;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::set_logslope;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::set_inflection;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::set_loggamma;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::maxinflection;
   using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::fadmap_stratum_active_d;
   //using par_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE >::FADGradComp;

   using Model_area::get_deltax_eq;
   using Model_area::get_deltay;
   using Model_area::get_m;
   using Model_area::get_n;
   using Model_area::get_cell_type;
   using Model_area::get_ilb;
   using Model_area::get_iub;
   using Model_area::long_to_index;
   using Model_area::lat_to_index;


 public:
   int ngrid;			///< number of usergrid regions
   int nseason;			///< number of seasons per year
   ivector startmonth;		///< vector of starting months for each season 
   ivector seasons;		///< season of each month
   MATRIX usergridU;		///< matrix of season/grid specific parameters
   MATRIX usergridV;		///< matrix of season/grid specific parameters
   MATRIX usergridSigma;	///< matrix of season/grid specific parameters
   imatrix stratum_active;	///< switch active parameters by region/season strata
   d3_array fadmap;           //fadmap matrices tree
   d3_array gx_fadmap;           //fadmap matrices tree
   d3_array gy_fadmap;           //fadmap matrices tree

 public:
   par_t_reg();			//par_treg.cpp
   par_t_reg(const par_t_reg < d3_array, dmatrix, dvector, double >&t);
   par_t_reg(const par_t_reg < dvar3_array, dvar_matrix, dvar_vector,
	     dvariable > &t);
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

   void uvcomp(MATRIX & u, MATRIX & v, const int season, const int t);
   void uvcomp(dvar_matrix & uSeasonGrid, dvar_matrix & vSeasonGrid,
	       dvar_matrix & u, dvar_matrix & v, const int season);

   void uvinterpolate(MATRIX & u, MATRIX & v, year_month & ym);
   void uvinterpolate(dvar_matrix & uSeasonGrid, dvar_matrix & vSeasonGrid,
		      dvar_matrix & u, dvar_matrix & v, const int season);

   void sigma_comp(MATRIX & sigma, year_month & ym);
   void sigma_comp(dvar_matrix & sigmaSeasonGrid, dvar_matrix & sigma,
		   const int season);

   void uvs_comp(MATRIX & u, MATRIX & v, MATRIX & sigma, year_month & ym);
   void uvs_comp(dvar_matrix & uSeasonGrid, dvar_matrix & vSeasonGrid,
		 dvar_matrix & sigmaSeasonGrid, dvar_matrix & u,
		 dvar_matrix & v, dvar_matrix & sigma, const int season);

   //dfuvs2.cpp
   void dfuvs_comp2(par_t_reg < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &param,
		    par_t_reg < d3_array, dmatrix, dvector,
		    double >&dfparam, dmatrix & dfu, dmatrix & dfv,
		    dmatrix & dfsigma, year_month & ym);

   //Virtual overwrites
   //nvarcalc.cpp
   int nvarcalc();
   //xinit3.cpp
   void xinit(dvector & x, adstring_array & xname);
   /*
   void reset(const dvar_vector & q, const dvariable & mort, const dvariable & special_mort,
	      const dvar_vector & report_rates, const dvar_vector & nb_par,
	      const dvar_matrix & seasonGridU,
	      const dvar_matrix & seasonGridV,
	      const dvar_matrix & seasonGridSigma,
	      const dvar_vector & min_D, 
              const dvar_vector & logslope,
	      const dvar_vector & inflection, 
              //const dvariable & sus_spd);
              const dvariable & chi);
   void reset(const model_parameters & parameters, dvariable & plike);
   //reset3.cpp
   void reset(const VECTOR & x);
   */
   //dfreset3.cpp
   void dfreset(dvector & g, const dvector & x);
   void dfreset(dvector & g, const dvar_vector & x) { }
   //rfgcomp.cpp
   void fgcomp(double &plike, dvector & x, dvector & g, int nvar,
	       recaptype_vector & recaps, int nrec,
	       coff_t < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &coff,
	       coff_t < d3_array, dmatrix, dvector, double >&dfcoff,
	       indexed_regional_fishery_record & irfr);

   //void fgcomp2(DOUBLE& plike, VECTOR& x, dvector& g, int nvar, recaptype_vector& recaps, int nrec, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr);

   //rsavefit.cpp
   void savefit(const double like, const int nvar, const double gmax,
		const char *_ext = NULL);

   void simulate(JNIEnv * env, jobject jobj, recaptype_vector & recaps,
		 int nrec, const adstring & path, const adstring & file);

   //for population simulator
   void population_simulate(indexed_regional_fishery_record & irfr,
			    JNIEnv * env, jobject jobj);
   void population_simulate(policy & pol,
			    indexed_regional_fishery_record & irfr,
			    JNIEnv * env, jobject jobj,
			    const adstring & path, const adstring & file);

   void reset_q(dvector & q_save, int &grow_time);
   void set_initial_q(dvector & q_save);
   void recruit_comp(dmatrix & recruitment, const dmatrix & pop,
		     const year_month & date, double &spawning_biomass);
   void population_initialize(dmatrix & pop, const double init_pop);

   // random walk simulators
   void walk(void);

   // halflife computation
   void halflife(indexed_regional_fishery_record & irfr);

   // densitysim computation
   void densitysim(indexed_regional_fishery_record & irfr);

   bool isRegional() const { return true; }

   dvariable computeObjectiveFunctionValue(model_parameters & parameters);

 private:			//Functions
   void allocate();		//par_treg.cpp
   void initialize();		//par_treg.cpp
   void set_seasons();		//par_seas.cpp

 public:			//Access Functions
   void set_usergrid_u(const int f, const int s, DOUBLE v) { usergridU[f][s] = v; }
   void set_usergrid_v(const int f, const int s, DOUBLE v) { usergridV[f][s] = v; }
   void set_usergrid_sigma(const int f, const int s, DOUBLE v) { usergridSigma[f][s] = v; }
   void set_nseason(int v) { nseason = v; }
   void set_ngrid(int v) { ngrid = v; }
   int get_season(year_month & date);
   int get_ngrid() const { return ngrid; }
   int get_nseason() const { return nseason; }
   int get_startmonth(int s) { return startmonth[s]; }
   DOUBLE get_usergrid_sigma(int f, int s) const { return usergridSigma[f][s]; }
   DOUBLE get_usergrid_u(int f, int s) const { return usergridU[f][s]; }
   DOUBLE get_usergrid_v(int f, int s) const { return usergridV[f][s]; }
   VECTOR get_seasonal_u(const int s);
   VECTOR get_seasonal_v(const int s);
   VECTOR get_seasonal_sigma(const int s);
   imatrix & get_stratum_active() { return stratum_active; }
   VECTOR & get_usergridU(const int s) { return usergridU[s]; }
   VECTOR & get_usergridV(const int s) { return usergridV[s]; }
   VECTOR & get_usergridSigma(const int s) { return usergridSigma[s]; }
};

template < typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE > 
ostream & operator <<(ostream & s, par_t_reg < D3_ARRAY, MATRIX, VECTOR, DOUBLE > &t);

#endif
