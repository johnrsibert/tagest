#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>


#ifdef __BORLANDC__
  #define PENDL endl
#endif
#ifdef __ZTC__
  #if (__ZTC__ <= 0x310)
    #define PENDL pendl
  #else
    #define PENDL pendl
  #endif
#endif
#ifdef __SUN__
  #define PENDL endl
#endif
#ifdef __GNUDOS__
  #define PENDL endl
#endif

#ifndef PRN_T
#define PRN_T
#ifndef FVAR_HPP
  class dvector;
  class ivector;
  class lvector;
  class dmatrix;
  class imatrix;
  class d3_array;
#endif

#ifndef __ADSTRING_HPP__
  class adstring;
#endif

class prnstream : protected ofstream
{
private:
  int count;
  void outstr(const char*, const char q = '\0');
public:
  prnstream(const char* fn);
  prnstream& put(char);
  void close(void) { ofstream::close();}

  prnstream& operator << (const char*);
  prnstream& operator << (const short);
  prnstream& operator << (const int);
  prnstream& operator << (const long);
  prnstream& operator << (const float);
  prnstream& operator << (const double);

  prnstream& operator<< (prnstream& (*fn)(prnstream &));
  prnstream& operator<< (ios& (*_f)(ios&));

  friend prnstream& endl(prnstream& s);

#if defined(__ZTC__) //&& (__ZTC__ <= 0x310)
  friend prnstream& pendl(prnstream& s);
  prnstream& operator << (const adstring& s);
#endif

  prnstream& operator<<(dvector& z);
  prnstream& operator<<(ivector& z);
  prnstream& operator<<(lvector& z);
  prnstream& operator<<(imatrix& z);
  prnstream& operator<<(dmatrix& z);
  prnstream& operator<<(d3_array& z);


};


#ifdef __BCPLUSPLUS__
 #if (__BCPLUSPLUS__ <= 0x310)
  inline prnstream& operator << (prnstream& s, const smanip_int& t) { return(s);}
 #else
  inline prnstream& operator << (prnstream& s, smanip<int> &t) { return s;}
//  #error Need to declare/implement inline prnstream& operator << (prnstream& s, const smanip_int& t) { return(s);}
 #endif
#endif

#ifdef __ZTC__                               //SMANIP(int) setw(int);
  #if (__ZTC__ <= 0x310)
    inline prnstream& operator<<(prnstream& s, SMANIP(int) &m) { return s;}
  #else
    inline prnstream& operator<<(prnstream& s, SMANIP<int> &m) { return s;}
  #endif
#endif

inline prnstream& prnstream::operator<< (prnstream& (*_f)(prnstream &))
					{ return (*_f)(*this); }

#endif // #ifndef PRN_T
