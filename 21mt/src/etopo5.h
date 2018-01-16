#ifndef __ETOPO5_H__
#define __ETOPO5_H__
#include <fstream>

class etopo5_matrix
{
public://Contructors and Destructors
  etopo5_matrix();
  ~etopo5_matrix();

private:
  ifstream s;
  int swLong, swLat;
  int current_latitude;
  short int * v;          //longitude array for a latitude
  int allocated_flag;

public:
  void allocate();
  int operator !() const { return (allocated_flag == 0);   }
  const int operator() (const int Long, const int Lat);
  const short* trace_v() const;
  void test();
};//etopo5_matrix
#endif
