//$Id: longtude.cpp 2949 2012-02-13 02:04:24Z jsibert $
#include <stdlib.h>
#include "longtude.h"
#include "trace.h"
#include <iomanip>

double Longitude::MAX_LONG_RANGE = 360.0;
double Longitude::MIN_LONG_RANGE = 0.0;

Longitude::Longitude(const char* _long): LongLat()
{
  char temp[10];
  strcpy(temp, _long);
 
  if (!strlen(temp))
  {
    cerr << "\nError: Null string passed to Longitude::Longitude(char*).\n";
    return;
  }
 
  if (temp[strlen(temp) - 1] != 'W' && temp[strlen(temp) - 1] != 'E' )
  {
    cerr << "\nError: Unable to construct Longitude::Longitude(const char*) "
         << "with char* \"" << _long << "\" .\n";
    return;
  }

  //remove symbol
  temp[strlen(temp) - 1] = '\0';
  
  if (!number_string(temp))
  {
    cerr << "\nError: Unable to construct Longitude::Longitude(const char*) "
         << "with non number_string char* \"" << _long << "\" .\n";
    return;
  }
  
  double coord = decimal(temp);  

  if (_long[strlen(_long) - 1] == 'W')
    coord = MAX_LONG_RANGE - coord;
  range(coord, "Longitude::Longitude(const char*)");
  coord_.set(MIN_LONG_RANGE, MAX_LONG_RANGE, coord);
}//End Longitude(const char*)

Longitude::Longitude(const double _long): LongLat()
{
  range(_long, "Longitude::Longitude(const char*)");
  coord_.set(MIN_LONG_RANGE, MAX_LONG_RANGE, _long);
}//End Longitude(const double)

istream& operator >> (istream& is, Longitude& c)
{
  char tmp1[10];
  is >> tmp1;
  if (is)
  {
    Longitude tt(tmp1);
    c = tt;
  }
  return(is);
}

ostream& operator << (ostream& os, const Longitude& l)
{
 if (os)
 {
    const ios::fmtflags osflags = os.flags();
    os.setf(ios_base::fixed);
    if (LongLat::input_mode == 1) // reading dd.mm
    {
       int d = int(l.value());
       int m = int((float)(60 * (l.value() - d)));

       if (l.value() < 180.0)
       {
         os << d;
         if (m > 0) os << "." << m;
         os << "E";
       }
       else
       {
         os << (360 - d);
         if (m > 0) os << "." << (60 - m);
         os << "W";
       }
    }
    else if (LongLat::input_mode == 2) // reading decimal degrees
    {
      //os << setw(7) << setprecision(2) << l.long_lat_value() << setw(1) << "E";
      os << setprecision(2) << l.long_lat_value() << setw(1) << "E";
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

void Longitude::range(const double _coord, const char* _func_name)
{
  if (_coord > MAX_LONG_RANGE)
  {
    cerr << "Error: longitude " << _coord
         << " out of range in " << _func_name << endl;
    cerr << "longitudes must be less than 360 degrees" << endl;
  }

  if (_coord < MIN_LONG_RANGE)
  {
    cerr << "Error: longitude " << _coord
         << " out of range in " << _func_name 
         << "\nlongitudes must be greater than 0 degrees" << endl;
  }
}//End range
