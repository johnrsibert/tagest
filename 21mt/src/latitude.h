#ifndef __LATITUDE_H__
#define __LATITUDE_H__
#include "longlat.h"

class Latitude: public LongLat
{
public:
  // Process: LongLat::LongLat() does all the work 
  Latitude(): LongLat() { }; 

  //  Input: accepts formats dd.mmC ddC for C in [n, N, s, S] 
  //         -> Error null string, but produces 0S
  //  Process: for accepted string call LongLat to convert to decimal 
  //           changes double LongLat::coord 
  //           -> Error for coord not in range, but produces 0S
  Latitude(const char*);

  /// Input: accepts decimal representation of Longitude in 
  ///        range [MIN_LAT_RANGE, MAX_LAT_RANGE]
  ///        -> Error out of range, but produces 0S
  Latitude(const double);
public://Method 
  Latitude& operator = (const Latitude& c){
    coord_ = c.coord_;
    return(*this);
  }
 
  int operator == (const Latitude& right) const {
    return coord_ == right.coord_;
  }

private://Method
  void range(const double, const char*);
  static double MAX_LAT_RANGE;
  static double MIN_LAT_RANGE;
};

istream& operator >> (istream& is, Latitude& l);
ostream& operator << (ostream& os, const Latitude& l);
#endif // #ifndef __LATITUDE_H__
