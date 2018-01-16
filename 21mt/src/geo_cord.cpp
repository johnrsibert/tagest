#include "geo_cord.h"
 
int Geo_coord::operator == ( const Geo_coord& gright ) const
{
  if ((longitude == gright.longitude) && (latitude == gright.latitude))
    return 1;
  else
    return 0;
}

istream& operator >> (istream& is, Geo_coord& g)
{
  Longitude tmp1;
  Latitude tmp2;
  is >> tmp1;
  is >> ws;
  is >> tmp2;
  if (is)
  {
    Geo_coord gg(tmp1, tmp2);
    g = gg;
  }
  return(is);
}

ostream& operator << (ostream& os, const Geo_coord& g)
{
  if (os)
  {
    os << g.get_long().value() << " " << g.get_lat().value();
  }
  return(os);
}
  
