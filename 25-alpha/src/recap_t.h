//$Id: recap_t.h 3201 2014-08-06 01:47:04Z jsibert $
#ifndef __recap_t_h__
#define __recap_t_h__
#include "yr_mon2.h"
#include "par_t.h"


class recaptype_vector;
/** Holds tag return data.*/
class recaptype 
{
private:
  //static int cohort_pooling_flag;
public:
  recaptype();
  char  fleet[5]; ///< 4 byte fleet name
  /** Index of fleet in irfr. Set in par_t::readcaps().
  The irfr structure may contains (many) more fleets than specified
  in .par file. */
  int   ifr_flindex;
  /** Index of fleet in par_t. Set in par_t::readcaps().
  Set to zero if fleet is not included in the par_t structure. */
  int   par_flindex;
  year_month  date; ///< Recapture date.
  /** Cohort number. Set acording to cohort pooling flag.  */
  int   cohort;
  int   i;  ///< Longtude index in model area coordinate system
  int   j;  ///< Latitude index in model area coordinate system
  int   returns; ///< Number of tags reported.
  /** Indicates effort in ifr. 
  1 => effort present; 0 => "effortless" recapture. Set in void readrecaps(...).
  */
  int   effort_flag;

  /*
  static int cohort_pooling(void)
  {
    return(cohort_pooling_flag);
  }
  static void set_cohort_pooling(const int t)
  {
    cohort_pooling_flag = t;
  }
  */
  void write_on(ostream& s, Model_area& m);

  void read_from(istream& s, Model_area& m);

};

typedef recaptype recaptype_;

struct recaptype_vrep
{
  recaptype_ * p;
  int en1;
  int en2;
  int nsize;
  int refcnt;
};

class recaptype_vector
{
  protected:
    recaptype_vrep * v;
    void allocate();
    int cohort_pooling_flag;
    double total_recaps;
    int nrec;
    int totalRecords;

  public:
    static recaptype_vector *instance;

    template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
    static recaptype_vector* createInstance(const adstring &filename, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE> &param, indexed_regional_fishery_record& irfr);
    static void deleteInstance() 
    {
      if (instance != 0)
      {
         delete instance;
         instance = 0;
      }
    } 

  public: 
    //friend class recaptype_matrix;
    recaptype_vector(int n1, int n2);
    recaptype_vector();
    recaptype_* base() { return v->p; }
    recaptype_& operator[] (const int i) const;
    recaptype_vector& operator=(recaptype_vector& x);
    ~recaptype_vector();
    inline recaptype_vector(recaptype_vector& x)
    {
      x.v->refcnt++;
      v = x.v;
    }
    inline void set_cohort_pooling(const int t) { cohort_pooling_flag = t; }
    inline int cohort_pooling(void) const { return(cohort_pooling_flag); }
    inline void set_total_recaps(const double t) { total_recaps = t; }
    inline double get_total_recaps(void) const { return(total_recaps); }

    inline void set_nrec(const int t) {nrec = t;}
    inline int get_nrec(void) const {return nrec;}

    inline int getTotalRecords() const { return totalRecords; }
    inline void setTotalRecords(const int totalRecords)
        { this->totalRecords = totalRecords; }

    inline int indexmin() const {return(v->en1);}
    inline int indexmax() const {return(v->en2);}
    int current_position;
    void getreturns(d3_array& returns, const year_month& date, const int cohort); 
    //void getreturns(d3_array& returns1, d3_array& returns0, const year_month& date, const int cohort); 
    inline void reset_position(void) {current_position = 1;}
    ivector count_effortless_recaps(const int, adstring_array&);
    void tabulate_effortless_recaps(ofstream& os, const adstring_array& fleet_list);
    //dvector count_recap_strata(const adstring_array& fleet_list,
    //                           const ivector& ilbv, const ivector& iubv,
    //                           const imatrix& jlbm, const imatrix& jubm);

    void compute_obs_history(imatrix& obs_hist, year_month& start_date);

    void make_effort_proxy(d4_array& effort, year_month& start_date,
                       const adstring_array& fleet_list);


};

ostream& operator << (ostream& s, recaptype& t);
int recaptype_compare(recaptype_& a, recaptype_& b);
int recaptype_compare(const void* a, const void* b);
void recaptype_sort(recaptype_vector& v, const int n);
int operator == (recaptype_& a, recaptype_& b);
recaptype_& operator += (recaptype_& a, recaptype_& b);
#endif  // __recap_t_h__
