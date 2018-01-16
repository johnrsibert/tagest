//$Id: latitude.cpp 2949 2012-02-13 02:04:24Z jsibert $
#include <string.h>
#include <stdlib.h>
#include "latitude.h"
#include "trace.h"
#include <iomanip>

double Latitude::MAX_LAT_RANGE = 90.0;
double Latitude::MIN_LAT_RANGE = -90.0;

Latitude::Latitude(const char* _lat): LongLat()
{
  char temp[10];
  strcpy(temp, _lat);

  if (!strlen(temp))
  {
    cerr << "\nError: Null string passed to Latitude::Latitude(const char*).\n";
    return;
  }

  if (temp[strlen(temp) - 1] != 'N' && 
      temp[strlen(temp) - 1] != 'S' ) 
  {
    cerr << "\nError: Unable to construct Latitude::Latitude(const char*) " 
         << "with char* \"" << _lat << "\" .\n";
    return;
  }
  //remove symbol
  temp[strlen(temp) - 1] = '\0';
  if (!number_string(temp))
  {
    cerr << "\nError: Unable to construct Latitude::Latitude(const char*) " 
         << "with non number_string char* \"" << _lat << "\" .\n";
    return;
  }
  
  double coord = decimal(temp);
  if (_lat[strlen(_lat) - 1] == 'S') coord *= -1;//jca posible bug
  range(coord, "Latitude::Latitude(const char*)");
  coord_.set(MIN_LAT_RANGE, MAX_LAT_RANGE, coord);
}//End Latitude(char*)

Latitude::Latitude(const double _lat): LongLat()
{
  range(_lat, "Latitude::Latitude(const char*)");
  coord_.set(MIN_LAT_RANGE, MAX_LAT_RANGE, _lat);
}//End Latitude(const double)

void Latitude::range(const double _coord, const char* _func_name)
{
  if (_coord > MAX_LAT_RANGE)
  {
    cerr << "Error: latitude " << _coord
         << " out of range in " << _func_name
         << "latitudes must be less than 90 degrees.\n";
  }
  if (_coord < MIN_LAT_RANGE)
  {
    cerr << "Error: latitude " << _coord
         << " out of range in " << _func_name
         << "latitudes must be greater than 0 degrees.\n";
  }
}//End range

istream& operator >> (istream& is, Latitude& l)
{
  char tmp1[10];
  is >> tmp1;
  if (is)
  {
    Latitude tt(tmp1);
    l = tt;
  }
  return(is);
}

ostream& operator << (ostream& os, const Latitude& l)
{
  if (os)
  {
    const ios::fmtflags osflags = os.flags();
    os.setf(ios_base::fixed);
    if (LongLat::input_mode == 1) // reading dd.mm
    {
      int d = int(l.value());
      if (d < 0) d *= -1;

      double dd = l.value();
      if (dd < 0) dd *= -1;

      int m = int((float)(60 * (dd - (int)dd)));
      if (l.value() < 0.0)
      {
        os << d;
        if (m > 0) os << "." << m;
        os << "S";
      }
      else
      {
        os << d;
        if (m > 0) os << "." << m;
        os << "N";
      }
    }
    else if (LongLat::input_mode == 2) // reading decimal degrees
    {
      //os << setw(6) << setprecision(2) << fabs(l.long_lat_value()) << setw(1);
      os << setprecision(2) << fabs(l.long_lat_value()) << setw(1);
      if (l.long_lat_value() < 0.0)
         os << "S";
      else
         os << "N";
    }
    else
    {
       cerr << "Error: unknown input mode, " << LongLat::input_mode 
            << ", in class LongLat." << endl;
       exit(1);
    } 
    os.flags(osflags);
  }
  return (os);
}

