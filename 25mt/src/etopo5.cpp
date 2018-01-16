//$Id: etopo5.cpp 2907 2011-12-30 23:44:07Z johnoel $
#include <fvar.hpp>
#include <adstring.hpp>
#include <stdlib.h>
#include "etopo5.h"
#include "trace.h"

#include <iostream>
#include <fstream>

using namespace std;

/** Constucts NULL instance.
*/
etopo5_matrix::etopo5_matrix()
{
  v = NULL;
  allocated_flag = 0;
  current_latitude = -1;
  swLong = swLat = 0;
}
  
/** Allocates memory and initializes data file.
Allocates 4320 elements to the array v.
Opens etopo5.dos in the directory pointed to by the "ETOPO_HOME"
environment variable.
*/
void etopo5_matrix::allocate()
{
  v = new short int [4320];
  if (!v)
  {
    cerr << "Error locating mememory for buffer in etopo5_matrix constructor."
         << endl;
    exit(1);
  }

  adstring envname("ETOPO_HOME");
  TRACE(envname)
  char* etopo_path=getenv(envname);
  if (!etopo_path)
  {
     cerr << envname << " not defined" << endl;
     exit(1);
  }


#ifdef unix
  adstring etopo_file = adstring(etopo_path) + adstring("/etopo5.dos");
  //s.open("../../data/etopo5.dos",ios::in);
#else
  /*
  #ifdef __ZTC__
    s.open("c:\\movemod\\etopo5.dos",0x80|ios::in);
  #else
    s.open("c:\\etopo5\\etopo5.dos",ios_base::binary|ios_base::in);
 #endif
  */
  adstring etopo_file = adstring(etopo_path) + adstring("\\etopo5.dos");
#endif
  TRACE(etopo_file)
  s.open(etopo_file, ios::in);
  if (!s)
  {
    cerr << "Error opening etopo5 data file ," << etopo_file << endl;
    exit(1);
  }
}

/** Returns the current value of v.
\return short int * containing a vector of elevations along a line of latitude.
*/
const short* etopo5_matrix::trace_v() const
{
  return(v);
}

/** Frees memory and closes data file.*/
etopo5_matrix::~etopo5_matrix()
{
  delete v;
  v = NULL;
  s.close();
}

/** Get elevation for specified longitude and latitude.
Reads complete data base for specifie latitude if the value of
Lat != \ref current_latitude. Hence when looping over a region, it is faster to
vary longutde faster (ie in the inner loop).
Bounds checking on Long and Lat activeted by the ADMB OPT_LIB compilation macro.
\param Long Longitude in 5 minute intervals; ie 12*degrees
\param Lat Latitude in 5 minute intervals; ie 12*degrees. North pole is latitude 0; south pole is latitude 12*2*90=2160.
\return int containing elevation in meters above sea level
*/
const int etopo5_matrix::operator() (const int Long, const int Lat)
{
  int iLong = Long % 4319;//jca6.10 to allow for 0.0W

#ifndef OPT_LIB
  if ( (Lat < 0) || (Lat > 2160))
  {
    cerr << "etopo5_matrix::operator() (const int Long, const int Lat) error:"
            "  Lat value out of range; you had " << Lat
         << "  valid range is 0 to 2160";
    exit(1);
  }

  if ( (iLong < 0) || (iLong >= 4320) )
  {
    cerr << "etopo5_matrix::operator() (const int Long, const int Lat) error:"
            "  Long value out of range; you had " << iLong
         << "  valid range is 0 to 4319";
    exit(1);
  }
#endif

  if (Lat == 2160) return 2180;//South Pole elevation

  if (Lat != current_latitude)
  {
    int to_read = 8640; // bytes to read
    long record_number = long(Lat)*long(to_read);
    s.seekg(record_number, ios::beg);

    if (!s)
    {
       cerr << "Error setting file position in "
	       "etopo5_matrix::operator(int Long, int Lat)";
       exit(1);
    }
    current_latitude = Lat;
    s.read((unsigned char*)v, 8640);
    int actual_count = s.gcount();
    if ( (!s) || (actual_count != to_read) )
    {
       cerr << "Error reading latitude buffer in "
	       "etopo5_matrix::operator(int Long, int Lat)"
               "Attempted to read " << to_read << " bytes; "
	       "actually read " << actual_count << " bytes.";
       exit(1);
    }
  }
  return v[iLong];
}//End operator()

void etopo5_matrix::test()
{
  for (int j = 0; j < 4320; j++)
    ASSERT((operator()(j, 0) == -4290));

  exit(1);
}
