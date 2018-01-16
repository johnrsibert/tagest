#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <adstring.hpp>
#include "etopo5.h"
#include "trace.h"


etopo5_matrix::etopo5_matrix()
{
  v = NULL;
  allocated_flag = 0;
  current_latitude = -1;
  swLong = swLat = 0;
}
  
void etopo5_matrix::allocate()
{
  v = new short int [4320];
  if (!v)
  {
    cerr << "Error locating mememory for buffer in etopo5_matrix constructor."           << endl;
    exit(1);
  }

  adstring etoponame;
#ifdef unix
  etoponame = adstring("/drive2/etopo5/etopo5.dos");
  s.open(etoponame,ios::in);
#else
  etoponame = adstring("c:\\movemod\\etopo5.dos");
  #ifdef __ZTC__
    s.open(etoponame,0x80|ios::in);
  #else
    s.open(etoponame,ios_base::binary|ios_base::in);
 #endif
#endif
  if (!s)
  {
    cerr << "Error opening etopo5 data file." << endl;
    exit(1);
  }
  cout << "Opened " << etoponame << endl;
  clogf << "Opened " << etoponame << endl;
}

const short* etopo5_matrix::trace_v() const
{
  return(v);
}

etopo5_matrix::~etopo5_matrix()
{
  delete v;
  v = NULL;
  s.close();
}

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
