#include "longlat.h"
 
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
  istrstream is1((char*)str1, strlen(str1));
  double num1 = 0.0;
  //read in and store numeric part of coordinate// 
  is1 >> num1;
  //cout << str1 << " - > " << num1 << endl;

  //modf splits a double num1 into integer and fractional parts//
  //modf then stores integer part in ipart and returns the fraction//
  deg_ = 0.0;
  min_ = 0.0;
  sec_ = 0.0;

  min_  = 100 * modf(num1, &deg_);
//    coord.set(MIN_LONG_RANGE, MAX_LONG_RANGE, ipart + f / 60);
//    coord.set(MIN_LAT_RANGE, MAX_LAT_RANGE, ipart + f / 60);
  return deg_ + min_/(double)60.0;
}//End decimal 
//End of file
