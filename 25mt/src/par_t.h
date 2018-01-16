//$Id: par_t.h 2981 2012-06-13 18:26:39Z jsibert $
#ifndef PAR_T_H
#define PAR_T_H
#pragma interface
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
#include "arrays.cpp"
class indexed_regional_fishery_record;
#ifdef __hppa__
  #define SHORT short
#else
  #define SHORT short
#endif
class recaptype_vector;
class model_parameters;

typedef struct // hold tag release details
{
  year_month  date;    // date of release
  float       tags;    // number of tags released
  short int   cohort;  // cohort number, depends in pooling flag
  short int   release_cohort;  // tag release cohort number
  short int   i;       // array x index in real world for tag release
  short int   j;       // array index in real world for tag release
  int cur_rel;
} tagreleasetype_;
declare(vector, tagreleasetype_);
ostream& operator << (ostream& s, tagreleasetype_& t);
istream& operator >> (istream& s, tagreleasetype_& t);
cifstream& operator >> (cifstream& s, tagreleasetype_& t);

class trv_ptr
{
public:
  tagreleasetype_vector* ptr;
  //friend class par_t;
  tagreleasetype_& operator[](int i) { return (*ptr)[i];}
  tagreleasetype_& operator()(int i) { return (*ptr)[i];}
};


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE> class coff_t;

/** Base class for model parameters.
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
class par_t : public Model_area
{
public:
  friend class model_parameters;

  static par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* instance;
  static par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* createInstance(const adstring &filename);
  static void deleteInstance();

public:
  adstring executable;
  
  int nmonth;              ///< number of months of tag return data
  int nrelease;            ///< number of tag releases
  int nfleet;              ///< number of fishing fleets
  trv_ptr tr;              ///< tag releases
  //dmatrix fadmap;          ///< matrix of fadmap
  //dmatrix gx_fadmap;        ///< gradient matrix of fadmap in x
  //dmatrix gy_fadmap;        ///< gradient matrix of fadmap in y
  ivector fadmap_stratum_active_d; ///< switch fad-d parameters on/off

  // "active" parameters:
  DOUBLE sus_spd;       ///< fish sustainable speed  
  double mean_size;     ///< mean size of fish  
  dvector max_fad;       ///< maximum number of fad
  VECTOR slope;         ///< slope for FAD-D equation
  VECTOR inflection;    ///< inflection for FAD-D equation
  //VECTOR min_D;         ///< minimum diffusion rate for FAD-D equation
  DOUBLE mort;		///< instantaneous natural mortality
  DOUBLE special_mort;	///< instantaneous natural mortality for special region
  VECTOR q;		///< catchability coefficient by fleet
  VECTOR report_rates;	///< reporting rates

  //Parameter Bounds
  double minafm,     maxafm;
  double minsig,     maxsig;
  double minmort,    maxmort;
  double minq,       maxq;
  double minr_rates, maxr_rates;
  double minvel,     maxvel;
  double minsus_spd,      maxsus_spd;
  //double minMin_D,   maxMin_D;
  double minslope,   maxslope;
  //double mininflection;   
  dvector maxinflection; //maxinflections are different depending on regions 

  DOUBLE fpen;

  double total_uv_penalty;
  double total_sigma_penalty;
  double total_negative_tag_penalty;
  double total_total_tag_penalty;
  double total_bounds_penalty;
  double total_likelihood_contribution;
  double objective_function_value;
  double maximum_gradient;
  VECTOR nb_par; // negative binomial parameter
  int nvar;

public://Variables
  ipar_t  m_ipar;       ///< vector m_ipars ipar[0] holds number of ipars
  adstring_array  fleet_names;  ///< vector of 4 byte fleet identifiers
  ivector DomesticFleetKludgeFlag;  ///<  if do domestic fleet kludge

  year_month  start_date;  ///< starting date

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
  VECTOR afm;

public://Functions
  par_t();                  //par_t.cpp
  par_t(cifstream& prn);    //ppar.cpp
  par_t(const char* name, const adstring parversion);  //pparead.cpp
  par_t(const par_t<d3_array,dmatrix,dvector,double>& t);
  par_t(const par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& t);
  virtual ~par_t();         //par_t.cpp

  void  set_fishing_effort_normalization(indexed_regional_fishery_record& irfr);

protected://Functions
  //par_t.cpp
  void allocate();
  void initialize();

public:
  // coh_collapse.cpp
  void collapse_cohorts(recaptype_vector& recaps, const int nrec);
  ///< combines tag cohorts by date of release

/* Compute fishing mortality from catchability and fishing effort.
\tparam f_mort 3D array over model domain by fleet. On return, the layers contain fishing mortality 
for each fleet.
\f$ F_{fij} = q_f*E_{fij}\f$
\param effort d3_array of observed fishing effort, \f$E_{fij}\f$ over model domain by fleet.
\param date year_month object containing the year and month of fishing effort
*/
void fish_mort_comp(D3_ARRAY& f_mort, const d3_array& effort, year_month& date);
//void fish_mort_comp(d3_array& f_mort, const d3_array& effort, year_month& date);
//void fish_mort_comp(dvar3_array& f_mort, const d3_array& effort, year_month& date);
void fish_mort_comp(D3_ARRAY& f_mort, D3_ARRAY& f_mort0, const d3_array& effort, 
                    year_month& date, const MATRIX& tag_density, const d3_array& recaps0, 
                    const dvector& mean_effort);

DOUBLE baranov_catch_f0(const DOUBLE& f1, const DOUBLE& f0, const DOUBLE& P, const DOUBLE& rr);
DOUBLE dbaranov_catch_df0(const DOUBLE& f1, const DOUBLE& f0, const DOUBLE& P, const DOUBLE& rr);
DOUBLE NR_baranov_f0(const double& C, const DOUBLE& f1, const DOUBLE& P, const DOUBLE& rr, const DOUBLE& init_f0);

void df_fish_mort_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, d3_array& dff_mort, d3_array& effort, year_month& date);

//void fish_mort_comp(dvar3_array &f_mort, const d3_array &effort, const dvar_vector &q);

void total_mort_comp(D3_ARRAY& f_mort, MATRIX& z);
void total_mort_comp(D3_ARRAY& f_mort1, D3_ARRAY& f_mort0, MATRIX& z);
void total_mort_comp(dvar3_array &f_mort, dvar_matrix &z, dvariable &mort, dvariable &special_mort);

  //psavefit.cpp
  void savefit(ofstream& pf, const char* fullname);

void pred_recapture_comp(MATRIX& pred_recaptures, const MATRIX& pred_tags, const MATRIX& z, const D3_ARRAY& fmort, const int fleet, const year_month& date);
void df_pred_recapture_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& pred_recaptures, dmatrix& dfpred_recaptures, dmatrix& pred_tags, dmatrix& dfpred_tags, dmatrix& z, dmatrix& dfz, d3_array& fmort, d3_array& dffmort, int fleet, year_month& date);

private://Functions
  //pparead.cpp
  void read_tagrelease(cifstream& istr);
  void read_bounds(cifstream& istr);
  void read_ipar(cifstream& istr);

public://Overloaded and Virtual functions

  virtual void uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma,
                        year_month& ym){};
  virtual int nvarcalc(){return 0;}
  virtual void xinit(dvector& x, adstring_array& xnames){};
  virtual void reset(const VECTOR& x) = 0;
  virtual void savefit(const double _like, const int _nvar, const double gmax, const char* _ext) = 0;
  void dfparset(par_t<d3_array,dmatrix,dvector,double>& dfparam)
  {
    cout << "Error: Base class dfparset called" << endl;
    exit(1);
  }
  void make_dfpar_t(const par_t<d3_array,dmatrix,dvector,double>& t)
  {
    cout << "Error: Base class make_dfpar_t called" << endl;
    exit(1);
  }
  void dfuvs_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfu,
                  dmatrix& dfv, dmatrix& dfsigma, year_month& ym)
  {
    cout << "Error: Base class dfuvs_comp called" << endl;
    exit(1);
  }
  virtual void fgcomp(double& plike, dvector& x, dvector& g, int nvar, recaptype_vector& recaps, int nrec, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr) = 0;
  //virtual void fgcomp2(DOUBLE& plike, VECTOR& x, dvector& g, int nvar, recaptype_vector& recaps, int nrec, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr) = 0;

virtual bool isRegional() const { return false; } 
virtual bool isNeuralNet() const { return false; } 
void set(model_parameters& mp) { }
/*
virtual dvariable computeObjectiveFunctionValue(model_parameters& parameters)
{
	std::cout << __FILE__ << ':' << __LINE__ << std::endl;
	return 0;
}
*/
virtual void reset(const model_parameters& parameters, dvariable& plike)
{
	std::cout << "Error: " << __FILE__ << ':' << __LINE__ << std::endl;
	exit(0);
}

public://Access Functions
  double get_objective_function_value()
  { return objective_function_value; }
  int get_nvar() { return nvar; }
  double get_maximum_gradient() { return maximum_gradient; }
  int get_nfleet() const { return nfleet; }
  int get_nrelease() { return nrelease; }
  int get_nmonth() { return nmonth; }
  int get_month(year_month& _date) { return _date.get_month_value(); }
  int get_no_of_ipars() { return 150; }
  float get_tr_tags(int tt) { return tr[tt].tags; }
  short int get_tr_cohort(int tc) { return tr[tc].cohort; }
  short int get_tr_release_cohort(int tc) { return tr[tc].release_cohort; }
  short int get_tr_i(int ti) { return tr[ti].i; }
  short int get_tr_j(int tj) { return tr[tj].j; }
VECTOR& get_q() { return q; }
VECTOR& get_report_rates() { return report_rates; }
DOUBLE get_mort() { return mort; }
DOUBLE get_special_mort() { return special_mort; }
DOUBLE get_q(int qi) { return q(qi); }
DOUBLE get_report_rates(const int gr) { return report_rates[gr]; }
  double get_total_bounds_penalty() { return total_bounds_penalty; }
  double get_total_uv_penalty() { return total_uv_penalty; }
  double get_total_sigma_penalty() { return total_sigma_penalty; }
  double get_total_total_tag_penalty() { return total_total_tag_penalty; }
  double get_total_negative_tag_penalty() { return total_negative_tag_penalty; }
  year_month& get_start_date() { return start_date; }
  year_month& get_tr_date(int td) { return tr[td].date; }
  DOUBLE get_sus_spd() { return sus_spd; }
  //VECTOR& get_min_D() { return min_D; }
  VECTOR& get_slope() { return slope; }
  VECTOR& get_inflection() { return inflection; }

  double get_minafm() { return minafm; }
  double get_maxafm() { return maxafm; }
  double get_minsig() { return minsig; }
  double get_maxsig() { return maxsig; }
  double get_minmort() const { return minmort; }
  double get_maxmort() const { return maxmort; }
  double get_minq() const { return minq; }
  double get_maxq() const { return maxq; }
  double get_minr_rates() { return minr_rates; }
  double get_maxr_rates() { return maxr_rates; }
  double get_minvel() { return minvel; }
  double get_maxvel() { return maxvel; }
  double get_minsus_spd() {return minsus_spd;}
  double get_maxsus_spd() {return maxsus_spd;}
  //double get_minMin_D() { return minMin_D; }
  //double get_maxMin_D() { return maxMin_D; }
  double get_minslope() { return minslope; }
  double get_maxslope() { return maxslope; }
  //double get_mininflection() { return mininflection; }
  dvector get_maxinflection() { return maxinflection; }

  //DOUBLE get_fpen() { return fpen; }
  void set_sus_spd(DOUBLE spd) { sus_spd = spd; }
  //void set_min_D(VECTOR d) { min_D = d; }
  void set_slope(VECTOR d) { slope = d; }
  void set_inflection(VECTOR d) { inflection = d; }
  //void set_fpen(DOUBLE d) { fpen = d; }
  void set_mort(DOUBLE mt) { mort = mt; }
  void set_special_mort(DOUBLE mt) { special_mort = mt; }
  void set_nmonth(int nmt) { nmonth = nmt; }
  void set_nfleet(int nft) { nfleet = nft; }
  void set_nrelease(int nrt) { nrelease = nrt; }
void set_report_rates(const int sr, DOUBLE vr) { report_rates[sr] = vr; }
  void set_q(const int qr, DOUBLE qt) { q[qr] = qt; }
  void set_total_uv_penalty(double t) { total_uv_penalty = t; }
  void set_total_sigma_penalty(double t) { total_sigma_penalty = t; }
  void set_total_negative_tag_penalty(double t)
        { total_negative_tag_penalty = t; }
  void set_total_total_tag_penalty(double t) { total_total_tag_penalty = t; }
  void set_total_bounds_penalty(double t) { total_bounds_penalty = t; }
  void set_tr(int cc, tagreleasetype_& rr) { tr[cc] = rr; }
void add_total_bounds_penalty(const double penalty) { total_bounds_penalty += penalty; }
void add_total_bounds_penalty(const dvariable penalty) { total_bounds_penalty += value(penalty); }
void add_total_uv_penalty(const double penalty) { total_uv_penalty += penalty; }
void add_total_uv_penalty(const dvariable penalty) { total_uv_penalty += value(penalty); }
void add_total_sigma_penalty(const double penalty) { total_sigma_penalty += penalty; }
void add_total_sigma_penalty(const dvariable penalty) { total_sigma_penalty += value(penalty); }
void add_total_negative_tag_penalty(const dvariable penalty) { total_negative_tag_penalty += value(penalty); }
void add_total_negative_tag_penalty(const double penalty) { total_negative_tag_penalty += penalty; }
void add_total_total_tag_penalty(const dvariable penalty) { total_total_tag_penalty += value(penalty); }
void add_total_total_tag_penalty(const double penalty) { total_total_tag_penalty += penalty; }
};

#ifndef __APPLE__
template class par_t<d3_array,dmatrix,dvector,double>;
template class par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
#endif

#endif //#ifdef PAR_T_H //
