//$Id: habitat.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef __HABITAT_H__
#define __HABITAT_H__

#include <fvar.hpp>
#include <math.h>
#include "geo_cord.h"
#include "yr_mon2.h"

/* This is a trimmed down version of the Model_area class
written specfically to deal with the HABITAT data. Shiham 8/2/2002 */

class habitat
{
public:  //constructors
  habitat();
  habitat(const char* fname);
  ~habitat();

  //functions
  bool readata(const char* fname);  //reading data
  double hdata(year_month _ym, Longitude _long, Latitude _lat);  //returns data from habitatarea matrix

private:
  //Functions
  double a_move_corner(const int i, const double dx, const int edge);
  int move_corner(const double x, const double dx, const int edge);



  void  set_vector_bounds();
  void allocate();

  int m;        // number of cols = longitudes = i
  int n;         //number of rows = latitudes = j
  double deltax;        //dx, a constant
  double deltay;        //dy a constant
  Geo_coord sw_coord;  // longitude & latitude of south west corner of grid

  int stt_yr, stt_mo;  //series start year and montth
  int end_yr, end_mo;   //series end year and end month
  year_month stt_ym;
  year_month end_ym;


  ivector ilb;         //lower and upper bounds for ragged model
  ivector iub;
  ivector jlb;
  ivector jub;

 public:
  double index_to_long(const int i);
  double index_to_lat(const int j);

  int long_to_index(const Longitude& lng);
  int long_to_index(const double lng);

  int lat_to_index(const Latitude& lat);
  int lat_to_index(const double lat);

  int mon_to_index(year_month &ym);


  d3_array habitatarea;  //data array

  int get_m() {return m;}
  int get_n() {return n;}
  year_month get_start_ym() {return stt_ym; }
  year_month get_end_ym() { return end_ym; }

  const ivector get_ilb() {return ilb; }
  const ivector get_iub() {return iub; }
  const ivector get_jlb() {return jlb; }
  const ivector get_jub()  {return jub; }

  const int get_stt_yr()  { return  stt_yr; }
  const int get_stt_mo() { return  stt_mo; }

 };
#endif // #ifdef __HABITAT_H__
