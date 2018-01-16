#ifndef __GEO_COORD_H__
#define __GEO_COORD_H__
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <strstream>
using std::istrstream;
using std::ostrstream;
 
#include "longtude.h"
#include "latitude.h"

#define TTRACE(o1,o2) cout << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #o1 " = " << o1<< ", " << #o2 " = " << o2 << endl;

//class used to handle a geographical coordinate (eg: 130E 25S)//
class Geo_coord
{
  public:
    Geo_coord() { } 
    Geo_coord(const Longitude& longt, const Latitude& lat)
    { longitude = longt; latitude = lat; }
    Geo_coord& operator= (const Geo_coord& g)
    { longitude = g.longitude; latitude = g.latitude; return(*this); }
    int operator== (const Geo_coord& gright) const;
    double long_value() const { return(longitude.long_lat_value());}
    double lat_value() const { return(latitude.long_lat_value());}
    Longitude get_long() const { return(longitude); }
    Latitude get_lat() const { return(latitude); }
  private:
    Longitude longitude;
    Latitude latitude;

};

istream& operator >> (istream& is, Geo_coord& c);
ostream& operator << (ostream& os, const Geo_coord& c);
#endif // #ifndef __GEO_COORD_H__

