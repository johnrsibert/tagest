#ifndef IPAR_T_H
#define IPAR_T_H
#include <fvar.hpp>
#include <adstring.hpp>
//#include <safe_mem.h>
#include <iomanip>

class ipar_t
{
  lvector v;
  adstring_array name;

public:

  ipar_t(const int n);
  ipar_t();
  ipar_t(const ipar_t& t);

  long& operator [] (const int i) { return ((long&)(v)(i));}
  long& operator () (const int i) { return ((long&)(v)(i));}
  ipar_t& operator = (const ipar_t& t);
  int operator ! (void) 
    { return(!v); }
    //{ return( (!v) || (!name) ); }

  adstring get_name(const int i);
  void set_name(const int i, const char* c);
  void allocate(const int n);

  ~ipar_t(){};
};

ostream& operator << (ostream& s, ipar_t& t);

#endif //#ifdef IPAR_T_H
