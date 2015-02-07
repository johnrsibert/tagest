#ifndef __LONGITUDE_H__
#define __LONGITUDE_H__
#include <iostream.h>
#include <string.h>
#include <ctype.h>
#include "longlat.h"

class Longitude : public LongLat
{
public://Constructors
  // Process: LongLat::LongLat() does all the work
  Longitude():LongLat() {/*Do Nothing here*/}; 

  //  Input: accepts formats ddd.mmC dddC ... d.mmC dC
  //         for C in [w, W, e, E]
  //         -> Error null string, but produces 0E 
  //  Process: for accepted string call LongLat to convert to decimal
  //           changes double LongLat::coord
  //           -> Error for coord not in range, produces 0E  
  Longitude(const char*);
  
  // Input: accepts decimal representation of Longitude in 
  //        range [MIN_LONG_RANGE, MAX_LONG_RANGE]
  //        -> Error out of range, but produces 0E
  Longitude(const double);

public://Methods
  Longitude& operator = (const Longitude& c){ 
    coord_ = c.coord_; 
    return(*this); 
  }

  int operator == (const Longitude& right) const { 
    return coord_ == right.coord_; 
  }

private://Methods
  // Purpose: produce error message double param is out of range 
  //          no change to members
  void range(const double, const char*); 
  static double MAX_LONG_RANGE;
  static double MIN_LONG_RANGE;
};

istream& operator >> (istream& is, Longitude& l);
ostream& operator << (ostream& os, const Longitude& l);
#endif // #ifndef __LONGITUDE_H__
