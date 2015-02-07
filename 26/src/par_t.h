//$Id: par_t.h 3130 2014-02-04 21:48:02Z johnoel $
#ifndef PAR_T_H
#define PAR_T_H

#ifndef __APPLE__
#pragma interface
#endif
//#include "coff_t.h"
#include <fvar.hpp>
#include <cifstrem.h>
#if (__GNUC__ >=3)
#include <iomanip>
#else
#include <iomanip.h>
#endif
#include "ipar.h"
#include "yr_mon2.h"
#include "modlarea.h"
class indexed_regional_fishery_record;
#ifdef __hppa__
#define SHORT short
#else
#define SHORT short
#endif
class recaptype_vector;
class model_parameters;

typedef struct			// hold tag release details
{
   year_month date;		// date of release
   float tags;			// number of tags released
   int cohort;			// cohort number, depends in pooling flag
   int release_cohort;		// tag release cohort number
   int i;			// array x index in real world for tag release
   int j;			// array index in real world for tag release
   int cur_rel;
} tagreleasetype;

typedef tagreleasetype tagreleasetype_;
struct tagreleasetype_vrep
{
   tagreleasetype_ *p;
   int en1;
   int en2;
   int nsize;
   int refcnt;
};

class trv_ptr;
class tagreleasetype_vector
{
   tagreleasetype_vrep *v;
   void allocate();
 public:
    friend class tagreleasetype_matrix;
    tagreleasetype_vector(const int n1, const int n2);
    tagreleasetype_vector();
   tagreleasetype_ *base()
   {
      return v->p;
   }
   tagreleasetype_ & operator[] (const int i);
   tagreleasetype_vector & operator=(const tagreleasetype_vector & x);
   ~tagreleasetype_vector();
   inline tagreleasetype_vector(const tagreleasetype_vector & x)
   {
      x.v->refcnt++;
      v = x.v;
   }
   friend void tagreleasetype_sort(trv_ptr & tr, const int n);
};

class trv_ptr
{
 public:
   tagreleasetype_vector * ptr;
   //friend class par_t;
   tagreleasetype_ & operator[] (const int i)
   {
      return (*ptr)[i];
   }
   tagreleasetype_ & operator() (const int i)
   {
      return (*ptr)[i];
   }
};

int tagreleasetype_compare(const void *_a, const void *_b);
int tagreleasetype_compare(tagreleasetype & a, tagreleasetype & b);
void tagreleasetype_sort(trv_ptr & tr, const int n);

ostream & operator <<(ostream & s, tagreleasetype_ & t);
istream & operator >>(istream & s, tagreleasetype_ & t);
cifstream & operator >>(cifstream & s, tagreleasetype_ & t);



/** Base class for model parameters.
*/
class par_t:public Model_area
{
 public:
   friend class model_parameters;

   static par_t *instance;
   static par_t *createInstance(const adstring & filename);
   static void deleteInstance();

 public:
    adstring executable;

   int nmonth;			///< number of months of tag return data
   int nrelease;		///< number of tag releases
   int nfleet;			///< number of fishing fleets
   trv_ptr tr;			///< tag releases
   //dmatrix fadmap;          ///< matrix of fadmap
   //dmatrix gx_fadmap;        ///< gradient matrix of fadmap in x
   //dmatrix gy_fadmap;        ///< gradient matrix of fadmap in y
   ivector fadmap_stratum_active_d;	///< switch fad-d parameters on/off

   // "active" parameters:
   //dvariable sus_spd;       ///< fish sustainable speed  
   dvariable chi;		///< taxis coefficient for FAD-uv equation  
   double max_gx;		///< maximum gradient in gx for FAD-uv equation  
   double max_gy;		///< maximum gradient in gx for FAD-uv equation  
   double max_g;		///< maximum gradient between max_gx and max_gy for FAD-uv equation  
   //dvector max_gx;       ///< maximum gradient in gx for FAD-uv equation  
   //dvector max_gy;       ///< maximum gradient in gx for FAD-uv equation  
   //double mean_size;     ///< mean size of fish  
   dvector max_fad;		///< maximum number of fad
   dvar_vector logslope;	///< log(slope) for FAD-D equation
   dvar_vector inflection;	///< inflection for FAD-D equation
   dvar_vector min_D;		///< minimum diffusion rate for FAD-D equation
   dvariable loggamma;		///< a parameter for exponential FAD-D equation;log(gamma)
   //dvar_vector loggamma;        ///< a parameter for exponential FAD-D equation;log(gamma)
   dvariable mort;		///< instantaneous natural mortality
   dvariable special_mort;	///< instantaneous natural mortality for special region
   dvar_vector q;		///< catchability coefficient by fleet
   dvar_vector report_rates;	///< reporting rates

   //Parameter Bounds
   double minafm, maxafm;
   double minsig, maxsig;
   double minmort, maxmort;
   double minq, maxq;
   double minr_rates, maxr_rates;
   double minvel, maxvel;
   //double minsus_spd,      maxsus_spd;
   double minchi, maxchi;
   double minlogslope, maxlogslope;
   double minmin_D, maxmin_D;
   double minloggamma, maxloggamma;
   //double mininflection;   
   dvector maxinflection;	//maxinflections are different depending on regions 

   dvariable fpen;

   double total_uv_penalty;
   double total_sigma_penalty;
   double total_negative_tag_penalty;
   double total_total_tag_penalty;
   double total_bounds_penalty;
   double total_likelihood_contribution;
   double objective_function_value;
   double maximum_gradient;
   dvar_vector nb_par;		// negative binomial parameter
   int nvar;

 public:			//Variables
    ipar_t m_ipar;		///< vector m_ipars ipar[0] holds number of ipars
   adstring_array fleet_names;	///< vector of 4 byte fleet identifiers
   ivector DomesticFleetKludgeFlag;	///<  if do domestic fleet kludge

   year_month start_date;	///< starting date

  /** Number of "anomalous" fishing mortality events.
  \deprecated Proved to be not useful. Replaced by missing effort
  algrorithms.
  */
   int num_afm;
   ivector afm_yr;
   ivector afm_mo;
   ivector afm_fl;
   ivector afm_i;
   ivector afm_j;
   dvar_vector afm;

 public:			//Functions
    par_t();			//par_t.cpp
    par_t(cifstream & prn);	//ppar.cpp
    par_t(const char *name, const adstring parversion);	//pparead.cpp
    par_t(const par_t &t);
    virtual ~ par_t();		//par_t.cpp

   void set_fishing_effort_normalization(indexed_regional_fishery_record &
					 irfr);

 protected:			//Functions
   //par_t.cpp
   void allocate();
   void initialize();

 public:
   // coh_collapse.cpp
   void collapse_cohorts(recaptype_vector & recaps, const int nrec);
   ///< combines tag cohorts by date of release

/* Compute fishing mortality from catchability and fishing effort.
\tparam f_mort 3D array over model domain by fleet. On return, the layers contain fishing mortality 
for each fleet.
\f$ F_{fij} = q_f*E_{fij}\f$
\param effort d3_array of observed fishing effort, \f$E_{fij}\f$ over model domain by fleet.
\param date year_month object containing the year and month of fishing effort
*/
   void fish_mort_comp(dvar3_array & f_mort, const d3_array & effort,
		       year_month & date);
//void fish_mort_comp(d3_array& f_mort, const d3_array& effort, year_month& date);
//void fish_mort_comp(dvar3_array& f_mort, const d3_array& effort, year_month& date);
   void fish_mort_comp(dvar3_array & f_mort, dvar3_array & f_mort0,
		       const d3_array & effort, year_month & date,
		       const dvar_matrix & tag_density,
		       const d3_array & recaps0,
		       const dvector & mean_effort);

   dvariable baranov_catch_f0(const dvariable & f1, const dvariable & f0,
			      const dvariable & P, const dvariable & rr);
   dvariable dbaranov_catch_df0(const dvariable & f1, const dvariable & f0,
				const dvariable & P, const dvariable & rr);
   dvariable NR_baranov_f0(const double &C, const dvariable & f1,
			   const dvariable & P, const dvariable & rr,
			   const dvariable & init_f0);

   void df_fish_mort_comp(par_t & dfparam, d3_array & dff_mort,
			  d3_array & effort, year_month & date);

//void fish_mort_comp(dvar3_array &f_mort, const d3_array &effort, const dvar_vector &q);

   void total_mort_comp(dvar3_array & f_mort, dvar_matrix & z);
   void total_mort_comp(dvar3_array & f_mort1, dvar3_array & f_mort0,
			dvar_matrix & z);
   void total_mort_comp(dvar3_array & f_mort, dvar_matrix & z,
			dvariable & mort, dvariable & special_mort);

   //psavefit.cpp
   void savefit(ofstream & pf, const char *fullname);

   void pred_recapture_comp(dvar_matrix & pred_recaptures,
			    const dvar_matrix & pred_tags,
			    const dvar_matrix & z,
			    const dvar3_array & fmort, const int fleet,
			    const year_month & date);
   void df_pred_recapture_comp(par_t & dfparam, dmatrix & pred_recaptures,
			       dmatrix & dfpred_recaptures,
			       dmatrix & pred_tags, dmatrix & dfpred_tags,
			       dmatrix & z, dmatrix & dfz,
			       d3_array & fmort, d3_array & dffmort,
			       int fleet, year_month & date);

 private:			//Functions
   //pparead.cpp
   void read_tagrelease(cifstream & istr);
   void read_bounds(cifstream & istr);
   void read_ipar(cifstream & istr);


 public:			//Access Functions
   double get_objective_function_value()
   {
      return objective_function_value;
   }
   int get_nvar()
   {
      return nvar;
   }
   double get_maximum_gradient()
   {
      return maximum_gradient;
   }
   int get_nfleet() const
   {
      return nfleet;
   }
   int get_nrelease()
   {
      return nrelease;
   }
   int get_nmonth()
   {
      return nmonth;
   }
   int get_month(year_month & _date)
   {
      return _date.get_month_value();
   }
   int get_no_of_ipars()
   {
      return 150;
   }
   float get_tr_tags(int tt)
   {
      return tr[tt].tags;
   }
   int get_tr_cohort(int tc)
   {
      return tr[tc].cohort;
   }
   int get_tr_release_cohort(int tc)
   {
      return tr[tc].release_cohort;
   }
   int get_tr_i(int ti)
   {
      return tr[ti].i;
   }
   int get_tr_j(int tj)
   {
      return tr[tj].j;
   }
   dvar_vector & get_q()
   {
      return q;
   }
   dvar_vector & get_report_rates()
   {
      return report_rates;
   }
   dvariable get_mort()
   {
      return mort;
   }
   dvariable get_special_mort()
   {
      return special_mort;
   }
   dvariable get_q(int qi)
   {
      return q(qi);
   }
   dvariable get_report_rates(const int gr)
   {
      return report_rates[gr];
   }
   double get_total_bounds_penalty()
   {
      return total_bounds_penalty;
   }
   double get_total_uv_penalty()
   {
      return total_uv_penalty;
   }
   double get_total_sigma_penalty()
   {
      return total_sigma_penalty;
   }
   double get_total_total_tag_penalty()
   {
      return total_total_tag_penalty;
   }
   double get_total_negative_tag_penalty()
   {
      return total_negative_tag_penalty;
   }
   year_month & get_start_date()
   {
      return start_date;
   }
   year_month & get_tr_date(int td)
   {
      return tr[td].date;
   }
   //dvariable get_sus_spd() { return sus_spd; }
   dvariable get_chi()
   {
      return chi;
   }
   dvar_vector & get_min_D()
   {
      return min_D;
   }
   dvar_vector & get_logslope()
   {
      return logslope;
   }
   dvar_vector & get_inflection()
   {
      return inflection;
   }
   //dvar_vector& get_loggamma() { return loggamma; }
   dvariable get_loggamma()
   {
      return loggamma;
   }

   double get_minafm()
   {
      return minafm;
   }
   double get_maxafm()
   {
      return maxafm;
   }
   double get_minsig()
   {
      return minsig;
   }
   double get_maxsig()
   {
      return maxsig;
   }
   double get_minmort() const
   {
      return minmort;
   }
   double get_maxmort() const
   {
      return maxmort;
   }
   double get_minq() const
   {
      return minq;
   }
   double get_maxq() const
   {
      return maxq;
   }
   double get_minr_rates()
   {
      return minr_rates;
   }
   double get_maxr_rates()
   {
      return maxr_rates;
   }
   double get_minvel()
   {
      return minvel;
   }
   double get_maxvel()
   {
      return maxvel;
   }
   //double get_minsus_spd() {return minsus_spd;}
   //double get_maxsus_spd() {return maxsus_spd;}
   double get_minchi()
   {
      return minchi;
   }
   double get_maxchi()
   {
      return maxchi;
   }
   double get_minmin_D()
   {
      return minmin_D;
   }
   double get_maxmin_D()
   {
      return maxmin_D;
   }
   double get_minlogslope()
   {
      return minlogslope;
   }
   double get_maxlogslope()
   {
      return maxlogslope;
   }
   double get_minloggamma()
   {
      return minloggamma;
   }
   double get_maxloggamma()
   {
      return maxloggamma;
   }
   //double get_mininflection() { return mininflection; }
   dvector get_maxinflection()
   {
      return maxinflection;
   }

   //dvariable get_fpen() { return fpen; }
   //void set_sus_spd(dvariable spd) { sus_spd = spd; }
   void set_chi(dvariable chi)
   {
      chi = chi;
   }
   void set_min_D(dvar_vector d)
   {
      min_D = d;
   }
   void set_logslope(dvar_vector d)
   {
      logslope = d;
   }
   void set_inflection(dvar_vector d)
   {
      inflection = d;
   }
   void set_loggamma(dvariable d)
   {
      loggamma = d;
   }
   //void set_loggamma(dvar_vector d) { loggamma = d; }
   //void set_fpen(dvariable d) { fpen = d; }
   void set_mort(dvariable mt)
   {
      mort = mt;
   }
   void set_special_mort(dvariable mt)
   {
      special_mort = mt;
   }
   void set_nmonth(int nmt)
   {
      nmonth = nmt;
   }
   void set_nfleet(int nft)
   {
      nfleet = nft;
   }
   void set_nrelease(int nrt)
   {
      nrelease = nrt;
   }
   void set_report_rates(const int sr, dvariable vr)
   {
      report_rates[sr] = vr;
   }
   void set_q(const int qr, dvariable qt)
   {
      q[qr] = qt;
   }
   void set_total_uv_penalty(double t)
   {
      total_uv_penalty = t;
   }
   void set_total_sigma_penalty(double t)
   {
      total_sigma_penalty = t;
   }
   void set_total_negative_tag_penalty(double t)
   {
      total_negative_tag_penalty = t;
   }
   void set_total_total_tag_penalty(double t)
   {
      total_total_tag_penalty = t;
   }
   void set_total_bounds_penalty(double t)
   {
      total_bounds_penalty = t;
   }
   void set_tr(int cc, tagreleasetype_ & rr)
   {
      tr[cc] = rr;
   }
   void add_total_bounds_penalty(const double penalty)
   {
      total_bounds_penalty += penalty;
   }
   void add_total_bounds_penalty(const dvariable penalty)
   {
      total_bounds_penalty += value(penalty);
   }
   void add_total_uv_penalty(const double penalty)
   {
      total_uv_penalty += penalty;
   }
   void add_total_uv_penalty(const dvariable penalty)
   {
      total_uv_penalty += value(penalty);
   }
   void add_total_sigma_penalty(const double penalty)
   {
      total_sigma_penalty += penalty;
   }
   void add_total_sigma_penalty(const dvariable penalty)
   {
      total_sigma_penalty += value(penalty);
   }
   void add_total_negative_tag_penalty(const dvariable penalty)
   {
      total_negative_tag_penalty += value(penalty);
   }
   void add_total_negative_tag_penalty(const double penalty)
   {
      total_negative_tag_penalty += penalty;
   }
   void add_total_total_tag_penalty(const dvariable penalty)
   {
      total_total_tag_penalty += value(penalty);
   }
   void add_total_total_tag_penalty(const double penalty)
   {
      total_total_tag_penalty += penalty;
   }
};

#endif				//#ifdef PAR_T_H //
