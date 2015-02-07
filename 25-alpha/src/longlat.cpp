//$Id: longlat.cpp 3127 2013-12-05 22:40:36Z jsibert $
#include "longlat.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sstream>

int LongLat::input_mode = 2;

LongLat::LongLat()
{
   deg_ = 0.0;
   min_ = 0.0;
   //dec_ = 0.0;
}

int LongLat::number_string(const char* _longlat)
{
  if (!isdigit(_longlat[0]))  return 0;

  for (int i = 0; _longlat[i] != '\0'; i++)
    if (!isdigit(_longlat[i]) && _longlat[i] != '.')
      return 0;
  return 1;
}//End number_string

double LongLat::decimal(const char* str1)
{
  //make an instance called is1, using up to len1 bytes of str1// 
  //istrstream is1((char*)str1, strlen(str1));
  istringstream is1(str1);
  double num1 = 0.0;
  //read in and store numeric part of coordinate// 
  is1 >> num1;
  //cout << str1 << " - > " << num1 << endl;

  deg_ = 0.0;
  min_ = 0.0;
  sec_ = 0.0;
  double dec = 0.0;
  if (input_mode == 1) // reading dd.mm
  {
    // modf splits a double num1 into integer and fractional parts
    // modf retuns franctional part stores the integer part in the location
    // pointed to by the second argument
    min_  = 100 * modf(num1, &deg_);
    dec = deg_ + min_/(double)60.0;
  }
  else if (input_mode == 2) // reading decimal degrees
  {
    deg_ = trunc(num1);
    min_ = trunc((num1-deg_)*60.0);
    dec = num1;
  }
  else
  {
    cerr << "Error: unknown input mode, " << input_mode 
         << ", in class LongLat." << endl;
    exit(1);
  }
  return (dec);
}//End decimal 
//End of file
