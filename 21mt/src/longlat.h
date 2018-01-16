#ifndef __LONGLAT_H__
#define __LONGLAT_H__
#include <iostream>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <strstream>
using std::istrstream;
using std::ostrstream;
#include "wrap_num.h"

//class used to handle either a longitude or latitude//
class LongLat
{
public:
  LongLat() { } //wrapping_num default constructor assigns coord = 0//
  double long_lat_value() const { return(coord_.get_value());}
  double value() const { return(coord_.get_value());}
  int operator > (const LongLat& t) { return(value() > t.value() ); }
  int operator < (const LongLat& t) { return(value() < t.value() ); }

protected://Methods
  //  Process: just determines if the contains only digits and '.' 
  //           the first char needs to be a digit
  //           no change to members
  //  Note: dd.. would work though not worth worring about
  //  Output: True 1, False 0 no change to members
  int number_string(const char*);  
  
  // Process: converts strings to decimal form then returns results
  //          no change to members 
  double decimal(const char*);

protected://Members
  Wrapping_num coord_;          //Decimal form of Longitude of Latitude
  double         deg_;          //Hold LongLat int parts in double for ease 
  double         min_;
  double         sec_;
};//class LongLat
#endif // #ifndef __LONGLAT_H__
