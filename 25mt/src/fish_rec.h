//$Id: fish_rec.h 2842 2011-08-15 22:35:14Z jsibert $
#if (__GNUC__ >=3)
  #include <iostream>
#else
  #include <iostream>
#endif
#include <string.h>
#include <fvar.hpp>
#include <adstring.hpp>
#include <fvar.hpp>
#include "par_t.h"
#include "yr_mon2.h"
#include "longlat.h"
#include "arrays.hpp"
#include "list2.cpp"
#include "shift.h"

#ifndef __FISH_REC_H__
#define __FISH_REC_H__

#ifdef unix
  #define DEFAULT_PATH "../"
#else
  #define DEFAULT_PATH "..\\"
#endif
class Model_area;
class recaptype;
class recaptype_vector;

class fishery_record /// Details of fishing event.
{
  private:
    char Fleet[5];
    year_month Date;
    Latitude Lat;
    Longitude Long;
    double eff;
    double ctch;
    char input_record[256];
    //double recaps;

  public:
    adstring fleet(void) const { return adstring(Fleet); }
    year_month date(void) const { return Date; }
    Latitude latitude(void) const { return Lat; }
    Longitude longitude(void) const { return Long; }
    Latitude get_latitude(void) const { return Lat; }
    Longitude get_longitude(void) const { return Long; }
    void set_longitude(const Longitude& lng) { Long = lng; }
    void set_latitude(const Latitude& lte) { Lat = lte; }
    adstring get_input_record(void) const { return input_record; }

    double effort(void) const { return eff; }
    double Catch(void) const { return ctch; }
    void set_fleet(const char* _fleet);
    istream& read_info(istream& istr);
    int operator == (fishery_record& t);
    fishery_record& operator += (fishery_record& t);
    fishery_record& operator = (fishery_record& t);
    //fishery_record& operator = (recaptype& t);

    friend istream& operator>>(istream& istr, fishery_record& t);
    friend ostream& operator<<(ostream& ostr, fishery_record& t);

  #if defined(__SUN__) || defined(__GNU__)
    friend int fishery_record_compare(const void* a, const void* b);
  #else
    friend int cdecl fishery_record_compare(const void* a, const void* b);
  #endif
    friend class regional_fishery_record;

};

typedef fishery_record fishery_record_; ///< typedef to enable readable names
declare(vector, fishery_record_); ///< Macro to create vector of fishery_record instances
void sort(fishery_record_vector& v, const int n); ///< Sorts elements of a fishery_record_vector
double total_effort(fishery_record_vector& v, const int n);
double total_catch(fishery_record_vector& v, const int n);

class regional_fishery_record /// Details of fishing event expressed in model domain
{
  private:
    //static ivector jlb;
    //static ivector jub;
    //static int sw_long;
    //static int sw_lat;
    static int statics_set;
    static par_t<d3_array,dmatrix,dvector,double>* pparam;
    int state;
    int    i;          ///< index of x coordinate (longitude) of fishing
    int    j;          ///< index of y coordinate (latitude) of fishing
    double effort;     ///< observed effort for above indices
    double Catch;      ///< observed catch for above indices

  public:
    static void set_sw_corner(par_t<d3_array,dmatrix,dvector,double>* _param);
    regional_fishery_record& operator = (regional_fishery_record& t);
    regional_fishery_record& operator = (fishery_record& t);
    int set(fishery_record& t);
    //int set2(fishery_record& t, double dx, double dy);
    int set3(const int _i, const int _j, const double efrt, const double ctch);
    int operator ! (void) const {return (state != 0);}
    //int operator == (recaptype& t);
    double get_effort(void) { return(effort); }
    int get_i(void) { return(i);}
    int get_j(void) { return(j);}

    friend istream& operator>>(istream& istr, regional_fishery_record& t);
    friend ostream& operator<<(ostream& ostr, regional_fishery_record& t);

    friend class fishery_record;
    friend class indexed_regional_fishery_record;
};
typedef regional_fishery_record regional_fishery_record_;
declare(vector, regional_fishery_record_);

class rfr_ptr
{
  regional_fishery_record_vector* ptr;
public:
  friend class indexed_regional_fishery_record;
  regional_fishery_record& operator[](int i) { return (*ptr)[i];}
  regional_fishery_record& operator()(int i) { return (*ptr)[i];}
};

/** Details of fishing events stored for rapid retrieval.
*/
class indexed_regional_fishery_record 
{
  private:
    adstring_array fleet_list;  ///< list of 4 byte fishing fleet codes
    i3_array index; ///< Index of first record pertaining to year, month and fleet
    i3_array nrec;  ///< Nuber of records following. 
    rfr_ptr fishery_records;
    dvector mean_effort;  ///< vector of mean effort for each fleet
    void allocate(void); //const int nfleet, const int first_year, const int last_year);
    int nfleet; ///<  number of fleets
    int first_year; ///< first year included
    int last_year; ///< last year included
    int rfrv_length; ///< number of fishing events in data structure
    int normalized_flag; ///< one if effort is normalized; 0 otherwise
    year_month_vector first_effort_date; ///< first recorded fishing effort by fleet
    ivector fleet_usage_table; ///< maps irfr fleet list to param fleet list
    year_month_vector last_effort_date; ///< last recorded fishing effort by fleet

    void mean_effort_compute(const int ncell);

public:
  static indexed_regional_fishery_record *instance;
  static indexed_regional_fishery_record* createInstance(const adstring &filename)
  {
     if (instance == 0) 
     {
        instance = new indexed_regional_fishery_record(filename);
     }
     return instance;
   }
   static void deleteInstance()
   {
     if (instance != 0)
     {
       delete instance;
       instance = 0;
     }
   }

 public:
   year_month last_date(void) { return(year_month(last_year,12)); }
   indexed_regional_fishery_record(void){}
   indexed_regional_fishery_record(indexed_regional_fishery_record& t);
   indexed_regional_fishery_record(const char* root_name,
                                   const char* path = DEFAULT_PATH);
   indexed_regional_fishery_record(par_t<d3_array,dmatrix,dvector,double>& param, const int y1, const int y2);
   indexed_regional_fishery_record(const adstring& path,
                                   par_t<d3_array,dmatrix,dvector,double>& param,
                                   const int year1,
                                   const int year2,
                                   List<Shift>& lse);

   // access functions
   int flindex (const adstring& fleet);
   int get_index(const int flindex, const int year, const int month) {return(index(flindex,year,month));}
   int get_nrec(const int flindex, const int year, const int month)  {return(nrec(flindex,year,month));}
   regional_fishery_record& get_record(const int n) {return(fishery_records(n));}

   void write(const char* root_name, const char* path = DEFAULT_PATH);
   void read(const char* root_name, const char* path = DEFAULT_PATH);

   int set_effort_to_recaps(adstring& fleet, year_month& date,
                            recaptype_vector& recaps, int recap_recs, 
                            par_t<d3_array,dmatrix,dvector,double>& ma);
   int set_effort_to_recaps(adstring& fleet, 
                            recaptype_vector& recaps, int recap_recs, 
                            par_t<d3_array,dmatrix,dvector,double>& ma);
   int set_effort_to_recaps(adstring& fleet, year_month& date,
                            recaptype_vector& recaps, int recap_recs, 
                            par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& ma);
   int set_effort_to_recaps(adstring& fleet, 
                            recaptype_vector& recaps, int recap_recs, 
                            par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& ma);

   int get_effort(adstring& fleet, year_month& date, dmatrix& t);
   int get_effort(int flindex, int year, int month, dmatrix& t);
   
   /** Gets catch. 
   \deprecated Never implemented.
   */
   int get_catch(adstring& fleet, year_month& date, dmatrix& t);
   int did_effort_occur(adstring& fleet, year_month& date);
   void normalize(void);
   void normalize(year_month& date1, year_month& date2);
   int is_normalized(void) const {return(normalized_flag==1);}

   void tabulate(void);
   void tabulate(ostream& s);

   friend indexed_regional_fishery_record
      monthly_average_ifr(const char* root_name, const int m, const int n,
                          const char* path = DEFAULT_PATH);

   /** Access mean effort.
   \param f int containing the fleet index.
   \return double containing the mean effort for fleet f
   */
   double get_mean_effort(const int f) const { return(mean_effort(f)); }
   /** Access mean effort vector.
   \return dvector of length nfleet containing the mean effort for each fleet
   */
   const dvector& get_mean_effort(void) const { return(mean_effort); }

   friend class effort_stream;

   int get_first_year() const { return first_year; }
   int get_last_year() const { return last_year; }

   inline year_month_vector& get_first_effort_date()
                       { return first_effort_date; }
   inline year_month_vector& get_last_effort_date()
                       { return last_effort_date; }
   inline adstring_array& get_fleet_list() { return fleet_list; }  

   void set_fleet_usage_table(const  adstring_array& pfleet_list);
   inline int get_fleet_usage(const int flindex) const 
       { return fleet_usage_table(flindex); }
};

#undef USE_EFFORT_STREAM
#ifdef USE_EFFORT_STREAM
/** Mimics an ifstream on an ifr. 
\deprecated This class seems not to used (except perhaps in 
par_t_reg::simulate(...) ). I'm not sure it has a reason to live.
*/
class effort_stream
{
  private:
    int flindex;
    int year;
    int month;
    int state;
    static indexed_regional_fishery_record* irfr_ptr;

  public:
    static void set_irfr_ptr(indexed_regional_fishery_record* ptr);
    effort_stream(const char* file_name);
    effort_stream& operator >> (dmatrix& eff);
    int operator ! () {return(state);}    // non-zero if state failed
    void clear(void) {state = 0;}
    void close(void){}
};
#endif //USE_EFFORT_STREAM

#endif //__FISH_REC_H__
