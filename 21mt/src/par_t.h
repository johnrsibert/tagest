#ifndef PAR_T_H
#define PAR_T_H
#pragma interface
#include <fvar.hpp>
#include <cifstrem.h>
#include <iomanip>
//#include <safe_mem.h>
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

adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
adstring& adstring_utils_toupper(adstring& ads);
adstring_array& adstring_utils_toupper(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

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
  friend class par_t;
  tagreleasetype_& operator[](int i) { return (*ptr)[i];}
  tagreleasetype_& operator()(int i) { return (*ptr)[i];}
};


// holds generic parameters   
class par_t : public Model_area
{
protected://Variables
  int nmonth;              // number of months of tag return data
  int nrelease;            // number of tag releases
  int nfleet;              // number of fishing fleets   
  trv_ptr tr;              // tag releases   
  year_month  start_date;  // starting date   

  // "active" parameters:   
  double mort;             // instantaneous natural mortality
  double      special_mort;// instantaneous natural mortality for special region
  dvector q;               // catchability coefficient   
  dvector report_rates;    // reporting rates   

  int num_afm;
  ivector afm_yr;
  ivector afm_mo;
  ivector afm_fl;
  ivector afm_i;
  ivector afm_j;
  dvector afm;


  //Parameter Bounds
  double minafm,     maxafm;
  double minsig,     maxsig;
  double minmort,    maxmort;
  double minq,       maxq;
  double minr_rates, maxr_rates;
  double minvel,     maxvel;
  double fpen;

  double total_uv_penalty;
  double total_sigma_penalty;
  double total_negative_tag_penalty;
  double total_total_tag_penalty;
  double total_bounds_penalty;
  double total_likelihood_contribution;
  double objective_function_value;
  double maximum_gradient;
  dvector nb_par; // negative binomial parameter
  int nvar;

public://Variables
  ipar_t    	  m_ipar;       //vector m_ipars ipar[0] holds number of ipars 
  adstring_array  fleet_names;  //vector of char[4] fleet identifiers 

public://Functions
  par_t();                  //par_t.cpp
  par_t(cifstream& prn);    //ppar.cpp
  par_t(const char* name);  //pparead.cpp
  par_t(const par_t& t);    //par_t.cpp
  virtual ~par_t();         //par_t.cpp

  void  set_fishing_effort_normalization(indexed_regional_fishery_record& irfr);

protected://Functions
  //par_t.cpp
  void allocate();   
  void initialize();

public:
  // coh_collapse.cpp
  // combines tag cohorts by date of release
  void collapse_cohorts(recaptype_vector& recaps, const int nrec);

  //fmortq.cpp
  void fish_mort_comp(d3_array& f_mort, const d3_array& effort, year_month& date);
  void df_fish_mort_comp(par_t& dfparam, d3_array& dff_mort, d3_array& effort,
                         year_month& date);
  //zmortcmp.cpp
  void total_mort_comp(d3_array& f_mort, dmatrix& z);

  //psavefit.cpp
  void savefit(ofstream& pf, const char* fullname);

  void pred_recapture_comp(dmatrix& pred_recaptures,
                         dmatrix& pred_tags, dmatrix& z, d3_array& fmort, 
                         int fleet, year_month& date);
  void df_pred_recapture_comp(par_t& dfparam,
			dmatrix& pred_recaptures, dmatrix& dfpred_recaptures,
			dmatrix& pred_tags, dmatrix& dfpred_tags,
			dmatrix& z, dmatrix& dfz,
			d3_array& fmort, d3_array& dffmort, int fleet, year_month& date);

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
  virtual void reset(dvector& x){};
  virtual void dfreset(dvector& g, dvector& x){};
//  virtual void savefit(const double like, const int _nvar, const char* _ext);


  void dfparset(par_t& dfparam) 
  { 
    cout << "Error: Base class dfparset called" << endl; 
    exit(1); 
  }
  void make_dfpar_t(const par_t& t)
  {
    cout << "Error: Base class make_dfpar_t called" << endl;
    exit(1); 
  }
  void dfuvs_comp(par_t& dfparam, dmatrix& dfu, 
                  dmatrix& dfv, dmatrix& dfsigma, year_month& ym) 
  {
    cout << "Error: Base class dfuvs_comp called" << endl;
    exit(1);
  }
  void fgcomp(double& plike, dvector& x, dvector& g, int nvar,
              recaptype_vector& recaps, int nrec,
              coff_t& coff, coff_t& dfcoff,
              indexed_regional_fishery_record& irfr)
  {
    cout << "Error: Base class fgcomp called" << endl;
    exit(1);
  }

public://Access Functions
  double get_objective_function_value() 
  { return objective_function_value; }
  int get_nvar() { return nvar; }
  double get_maximum_gradient() { return maximum_gradient; }
  int get_nfleet() { return nfleet; }
  int get_nrelease() { return nrelease; }
  int get_nmonth() { return nmonth; }
  int get_month(year_month& _date) { return _date.get_month_value(); }
  int get_no_of_ipars() { return 150; }
  float get_tr_tags(int tt) { return tr[tt].tags; }
  short int get_tr_cohort(int tc) { return tr[tc].cohort; }
  short int get_tr_release_cohort(int tc) { return tr[tc].release_cohort; }
  short int get_tr_i(int ti) { return tr[ti].i; }
  short int get_tr_j(int tj) { return tr[tj].j; }
  dvector& get_q() { return q; }
  dvector& get_report_rates() { return report_rates; }
  double get_mort() { return mort; }
  double get_special_mort() { return special_mort; }
  double get_q(int qi) { return q(qi); }
  double get_report_rates(int gr) { return report_rates[gr]; }
  double get_total_bounds_penalty() { return total_bounds_penalty; }
  double get_total_uv_penalty() { return total_uv_penalty; }
  double get_total_sigma_penalty() { return total_sigma_penalty; }
  double get_total_total_tag_penalty() { return total_total_tag_penalty; }
  double get_total_negative_tag_penalty() { return total_negative_tag_penalty; }
  year_month& get_start_date() { return start_date; }
  year_month& get_tr_date(int td) { return tr[td].date; }

  double get_minafm() { return minafm; }
  double get_maxafm() { return maxafm; }
  double get_minsig() { return minsig; }
  double get_maxsig() { return maxsig; }
  double get_minmort() { return minmort; }
  double get_maxmort() { return maxmort; } 
  double get_minq() { return minq; }
  double get_maxq() { return maxq; }
  double get_minr_rates() { return minr_rates; } 
  double get_maxr_rates() { return maxr_rates; }
  double get_minvel() { return minvel; }
  double get_maxvel() { return maxvel; }
  double get_fpen() { return fpen; }

  void set_fpen(double i) { fpen = i; }
  void set_mort(double mt) { mort = mt; }
  void set_special_mort(double mt) { special_mort = mt; }
  void set_nmonth(int nmt) { nmonth = nmt; }
  void set_nfleet(int nft) { nfleet = nft; }
  void set_nrelease(int nrt) { nrelease = nrt; }
  void set_report_rates(int sr, double vr) { report_rates[sr] = vr; }
  void set_q(int qr, double qt) { q[qr] = qt; }
  void set_total_uv_penalty(double t) { total_uv_penalty = t; }
  void set_total_sigma_penalty(double t) { total_sigma_penalty = t; }
  void set_total_negative_tag_penalty(double t) 
        { total_negative_tag_penalty = t; }
  void set_total_total_tag_penalty(double t) { total_total_tag_penalty = t; }
  void set_total_bounds_penalty(double t) { total_bounds_penalty = t; }
  void set_tr(int cc, tagreleasetype_& rr) { tr[cc] = rr; }
}; //end of par_t class //

#endif //#ifdef PAR_T_H //
