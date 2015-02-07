//$Id: etopo5.h 2820 2011-05-30 21:13:11Z jsibert $
#ifndef __ETOPO5_H__
#define __ETOPO5_H__
#include <fstream>

/** 
Class for efficient handling of topographic data from the etopo5 
topographic data base. http://www.ngdc.noaa.gov/mgg/global/etopo5.HTML
*/
class etopo5_matrix
{
public://Contructors and Destructors
  etopo5_matrix();
  ~etopo5_matrix();

private:
  std::ifstream s;
  int swLong;           ///< Southwest corner of the region of interest. Not used?
  int swLat;            ///< Southwest corner of the region of interest. Not used?
  int current_latitude; ///< Place holder in the etopo5 data file for current latitute
  short int * v;        ///< C array containing all elevations along a line of longitude for the current_latitude.
  int allocated_flag;   ///< Set to one on allocation

public:
  void allocate();
  int operator !() const { return (allocated_flag == 0);   }
  const int operator() (const int Long, const int Lat);
  const short* trace_v() const;
  void test();
};//etopo5_matrix
#endif
