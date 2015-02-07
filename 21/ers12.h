#ifndef __ERS12_H__
#define __ERS12_H__

#include <fvar.hpp>
#include <math.h>
#include "geo_cord.h"

/* This is a trimmed down version of the Model_area class
written specfically to deal with the ERS12 data. msa 2/5/2002 */

class ers12
{
public:  //constructors
  ers12();
  ers12(const char* fname);
  ~ers12();
  
   //functions
  bool  readata(int yr, int mo);  //reading data
  double wnd(Longitude _long, Latitude _lat);  //returns data from ers12area matrix

private:
  //Functions
  double a_move_corner(const int i, const double dx, const int edge);  //inverse translation
  int move_corner(const double x, const double dx, const int edge);   // translates longitude or latitude end equator-dateline system to subscript

  double index_to_long(const int i);
  double index_to_lat(const int j);

  int long_to_index(const Longitude& lng);
  int long_to_index(const double lng);

  int lat_to_index(const Latitude& lat);
  int lat_to_index(const double lat);

  adstring ersfname (int yr, int mth);  //reads the names of the files
  void  set_vector_bounds();
  void allocate();

  //members
  int m;        // number of cols = longitudes = i 
  int n;         //number of rows = latitudes = j
  double deltax;              // dx, a constant
  double deltay;              // dy a constant 
  Geo_coord sw_coord;  // longitude & latitude of south west corner of grid 
  
  ivector ilb;         //lower and upper bounds for ragged model
  ivector iub;    
  ivector jlb;
  ivector jub;

  //ers12area can be defined as a d3_array
  //in this case start date and the nmonths has to be read from the information
  //file to the correct time dimension.. This should be trivial!
  dmatrix ers12area;  //data matrix

  ifstream s;
  
 };
#endif // #ifdef __ERS12_H__
