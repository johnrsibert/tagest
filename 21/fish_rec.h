#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <fvar.hpp>
#include <adstring.hpp>
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
class par_t;
class par_t_reg;
class recaptype;
class recaptype_vector;
class fishery_record
{
  private:
    char Fleet[5];
    year_month Date;
    Latitude Lat;
    Longitude Long;
    double eff;
    double ctch;
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

    double effort(void) const { return eff; }
    double Catch(void) const { return ctch; }
    void set_fleet(const char* _fleet);
    istream& read_info(istream& istr);
    int operator == (fishery_record& t);
    fishery_record& operator += (fishery_record& t);
    fishery_record& operator = (fishery_record& t);
    fishery_record& operator = (recaptype& t);

    friend istream& operator>>(istream& istr, fishery_record& t);
    friend ostream& operator<<(ostream& ostr, fishery_record& t);

  #if defined(__SUN__) || defined(__GNU__)
    friend int fishery_record_compare(const void* a, const void* b);
  #else
    friend int cdecl fishery_record_compare(const void* a, const void* b);
  #endif
    friend class regional_fishery_record;

};

typedef fishery_record fishery_record_;
declare(vector, fishery_record_);
void sort(fishery_record_vector& v, const int n);
double total_effort(fishery_record_vector& v, const int n);
double total_catch(fishery_record_vector& v, const int n);

class regional_fishery_record
{
  private:
    //static ivector jlb;
    //static ivector jub;
    //static int sw_long;
    //static int sw_lat;
    static int statics_set;
    static par_t* pparam;
    int state;
    int    i;          // index of x coordinate of fishing
    int    j;          // index of y coordinate of fishing
    double effort;     // observed effort for above indices
    double Catch;      // observed catch for above indices

  public:
    static void set_sw_corner(par_t* _param); //(Longitude& _long, Latitude& _lat);
    regional_fishery_record& operator = (regional_fishery_record& t);
    regional_fishery_record& operator = (fishery_record& t);
    int set(fishery_record& t);
    //int set2(fishery_record& t, double dx, double dy);
    int set3(const int _i, const int _j, const double efrt, const double ctch);
    int operator ! (void) const {return (state != 0);}
    //int operator == (recaptype& t);
    double get_effort(void) { return(effort); }

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

class indexed_regional_fishery_record
{
  private:
    adstring_array fleet_list;
    i3_array index;
    i3_array nrec;
    rfr_ptr fishery_records;
    dvector mean_effort;
    void allocate(void); //const int nfleet, const int first_year, const int last_year);
    int nfleet;
    int first_year;
    int last_year;
    int rfrv_length;
    int normalized_flag;
    void mean_effort_compute(void); //(year_month& date1, year_month& date2);
    //void mean_effort_compute(const int ncell); //(year_month& date1, year_month& date2);
    //friend class irfrStratumIterator;
    //friend class StratumIterator;

 public:
   year_month last_date(void) { return(year_month(last_year,12)); }
   indexed_regional_fishery_record(void){}
   indexed_regional_fishery_record(indexed_regional_fishery_record& t);
   indexed_regional_fishery_record(const char* root_name,
                                   const char* path = DEFAULT_PATH);
   indexed_regional_fishery_record(par_t& param, const int y1, const int y2);
   indexed_regional_fishery_record(const adstring& path,
                                   par_t& param,
                                   const int year1,
                                   const int year2,
                                   List<Shift>& lse);

   void write(const char* root_name, const char* path = DEFAULT_PATH);
   void read(const char* root_name, const char* path = DEFAULT_PATH);

   int set_effort_to_recaps(adstring& fleet, year_month& date,
                            recaptype_vector& recaps, int recap_recs, 
                            par_t& ma);
   int set_effort_to_recaps(adstring& fleet, 
                            recaptype_vector& recaps, int recap_recs, 
                            par_t& ma);

   int get_effort(adstring& fleet, year_month& date, dmatrix& t);
   int get_effort(int flindex, int year, int month, dmatrix& t);
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

   double get_mean_effort(const int f) const { return(mean_effort(f)); }
   const dvector& get_mean_effort(void) const { return(mean_effort); }

   friend class effort_stream;
    /*
    int ppp;
    class StratumIterator
    {
       private:
         int n1;
         int n2;
         int n;
         friend class indexed_regional_fishery_record;

       public:
         StratumIterator(void);
         void reset (adstring& fleet, year_month& date);
         void reset(void);
         int more(void){ n = this->ppp; return(n);}
         regional_fishery_record& next(void);
     };
     indexed_regional_fishery_record::StratumIterator* sip;
    */
};

/*
class irfrStratumIterator
{
  private:
    int n1;
    int n2;
    int n;
    indexed_regional_fishery_record* irfr;
  public:
    irfrStratumIterator(indexed_regional_fishery_record* t)
      {irfr = t; }
    void reset (adstring& fleet, year_month& date);
    void reset(void){ n = n1; }
    int more(void);
    regional_fishery_record& next(void);
    //double next_effort(void);
};
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

#endif //__FISH_REC_H__
