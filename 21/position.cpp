#include <stdlib.h>
#include <ctype.h>
#include <iostream.h>
#include "modlarea.h"

// convert geographic latitude to "equator/dateline" convention
void lat_trans(int& lat, const char code)
{
  char c = (char)toupper(code);

  if (c == 'S')
  {
    //Don't set the sign bit of 0 latitude.
    if ( lat != 0 )
      lat = -lat;
  }
  else if (c != 'N')
  {
    cerr << "Illegal latitude code ('" << code
	 << "') passed to lat_trans." << endl;
    cerr << "Legal latitude codes are 'S', 's', 'N', or 'n'." << endl;
    exit(1);
  }
}

// convert geographic longitude to "equator/dateline" convention
void long_trans(int& longt, const char code)
{
  char c = (char)toupper(code);
  if (c== 'E')
	{
		#if defined(__ATLANTIC__)
    longt = longt;
		#else
    longt = -(180-longt);
		#endif
	}
  else if (c== 'W')
	{
		#if defined(__ATLANTIC__)
    longt = -(longt);
		#else
    longt = -(longt-180);
		#endif
	}
  else
  {
    cerr << "Illegal longitude code ('" << code
	 << "') passed to llong_trans." << endl;
    cerr << "Legal latitude codes are 'E', 'e', 'W', or 'w'." << endl;
    exit(1);
  }
}


// translates longitude or latitude end equator-dateline system to subscript
int Model_area::move_corner(const double x, const double dx, const int edge)
{
  double tmp = 60.0*(x-edge)/dx + 1e-5;
  return ((int)tmp+1);
}

// inverse translation
double Model_area::a_move_corner(const int i, const double dx, const int edge)
{
  double tmp = edge + (double)(i-1)*dx/60.0;  //dx*(double)(i-1+edge)/60.0;
  return(tmp);
}  

int Model_area::long_to_index(const double lng)
{
  int i = move_corner(lng, deltax, sw_coord.long_value());
  return(i);
}

int Model_area::long_to_index(const Longitude& lng)
{
   return(long_to_index(lng.value()));
}

int Model_area::lat_to_index(const double lat)
{
  int j = move_corner(lat, deltay, sw_coord.lat_value());
  return(j);
}

int Model_area::lat_to_index(const Latitude& lat)
{
   return(lat_to_index(lat.value()));
}

double Model_area::index_to_long(const int i)
{
  double lng = a_move_corner(i, deltax, sw_coord.long_value());
  return(lng);
}

double Model_area::index_to_lat(const int j)
{
  double lat = a_move_corner(j, deltay, sw_coord.lat_value());
  return(lat);
}  

