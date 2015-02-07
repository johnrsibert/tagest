//$Id: ipar.h 2949 2012-02-13 02:04:24Z jsibert $
/** \file ipar.h
Defines a "flag" class.
\todo Rewrite ipar_t class as a derived class of ivector.
*/
#ifndef IPAR_T_H
#define IPAR_T_H
#include <fvar.hpp>
#include <adstring.hpp>
#if (__GNUC__ >=3)
  #include <iomanip>
#else
  #include <iomanip.h>
#endif

class ipar_t
{
  lvector v;
  adstring_array name;

public:

  ipar_t(const int n);
  ipar_t();
  ipar_t(const ipar_t& t);

  int indexmin(void){return v.indexmin();}
  int indexmax(void){return v.indexmax();}

  long& operator [] (const int i) const { return ((long&)(v)(i));}
  long& operator () (const int i) const { return ((long&)(v)(i));}
  ipar_t& operator = (const ipar_t& t);
  int operator ! (void) { return(!v); }

  adstring get_name(const int i);

  void set_name(const int i, const char* c);
  void allocate(const int n);

  ~ipar_t(){};
};

ostream& operator << (ostream& s, ipar_t& t);

#endif //#ifdef IPAR_T_H
