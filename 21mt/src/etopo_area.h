#ifndef __GRIDMAP_H__
#define __GRIDMAP_H__
#include <iostream>
#include <cifstrem.h>
#include <math.h>
#include <iomanip>
#include <fvar.hpp>
#include <adstring.hpp>

#include "modlarea.h"
#include "etopo5.h"
#include "latitude.h"
#include "longtude.h"

class etopo_area : public Model_area
{
public://Functions
  etopo_area();
  etopo_area(const char* prnfile);

  etopo_area(int _minuteElevation,
          int _m, int _n,
          int _minutesX, int _minutesY,
          Longitude _swLong, Latitude _swLat);

  etopo_area(etopo_area&)
  {
    cerr << "\nError: etopo_area::etopo_area(etopo_area&) not implemented.\n";
    exit(1);
  }

  ~etopo_area() { /*Nothing to clean up.*/ }

public://Functions

  //////////////////////////////////////////////////////
  // Input: _igrid region usually from 1 to n
  //        error if _igrid not found in gridmap_
  // Process: Find center of region
  // Output: Parameters _long and _lat are changed
  void CenterCoordinate(const int _igrid, Longitude& _long, Latitude& _lat);

  //////////////////////////////////////////////////////
  //return elevation in whole meters
  int elevation(Longitude _long, Latitude _lat);

  //////////////////////////////////////////////////////
  //return Long/Lat the sw corner of a block
  char* long_block(Longitude _long, char* _buffer);
  char* lat_block(Latitude _lat, char* _buffer);

  //////////////////////////////////////////////////////
  //return long/lat for index in gridmap
  char* long_gridmap(int _mi, char* _buffer);
  char* lat_gridmap(int _nj, char* _buffer);

  //////////////////////////////////////////////////////
  /// Purpose: Return number of 5min squares above ELEVATION_
  /// Note: _LONG, _LAT is converted to sw_corner of the
  ///      the block if not already
  int land(Longitude _long, Latitude _lat);

  //////////////////////////////////////////////////////
  /// General functions
  int min_elevation(Longitude _long, Latitude _lat);
  int max_elevation(Longitude _long, Latitude _lat);
  double avg_elevation(Longitude _long, Latitude _lat);

  //////////////////////////////////////////////////////
  /// Purpose: inserts 0 to GRIDMAP_  if the number of 5min
  ///          exceeds _PERCENT_BLOCKS_ABOVE_ELEVATION
  void insert_land_gridmap(const double _percent_blocks_above_elvation);

  //////////////////////////////////////////////////////
  // counts the number of 5 min blocks with depth between
  // threshold  and zero
  //
  void shoals(const int threshold);

  int write_matrix(ostream& s, const dmatrix& t);
  int write_matrix(ostream& s, const imatrix& t);

  void test_1();

public://Access functions
  void set_threshold_elevation(int _meters){ threshold_elevation = _meters;}

  //imatrix& get_gridmap() { return gridmap_; }

private://Variables
  etopo5_matrix em;
  int threshold_elevation;     // +/- whole meters
  int delx, dely;   //number of 5 minute blocks in each direction
  
private://Functions
  //////////////////////////////////////////////////////
  /// Purpose:  Convert to index of etopo5_matrix (*12 for 5 min)
  int xindex(const Longitude& _long);
  int yindex(const Latitude& _lat);

  int xindex(const int long_index);
  int yindex(const int lat_index);
};//End etopo_area

ostream& operator << (ostream& os, etopo_area& l);

#endif
