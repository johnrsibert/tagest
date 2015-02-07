//$Id: prnstrem.h 3127 2013-12-05 22:40:36Z jsibert $
#ifndef PRN_T
#define PRN_T
#include <sstream>
#include <fstream>

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

/** Stream output to create comma-separated variables in a (".csv") file.
csvstream is a synonym via a typedef.
*/
class prnstream : protected std::ofstream
{
private:
  int count;
  void outstr(const char*, const char q = '\0');
  void outstr(const std::ostringstream&, const char q = '\0');
public:
  prnstream(const char* fn);
  prnstream& put(char);
  void close(void) { std::ofstream::close();}

  prnstream& operator << (const char*);
  prnstream& operator << (const short);
  prnstream& operator << (const int);
  prnstream& operator << (const long);
  prnstream& operator << (const float);
  prnstream& operator << (const double);

  prnstream& operator<< (prnstream& (*fn)(prnstream &));
  prnstream& operator<< (std::ios& (*_f)(std::ios&));

  friend prnstream& endl(prnstream& s);

/*
#if defined(__ZTC__) //&& (__ZTC__ <= 0x310)
  friend prnstream& pendl(prnstream& s);
  prnstream& operator << (const adstring& s);
#endif
*/
  prnstream& operator << (const adstring& s);
  prnstream& operator<<(dvector& z);
  prnstream& operator<<(ivector& z);
  prnstream& operator<<(lvector& z);
  prnstream& operator<<(imatrix& z);
  prnstream& operator<<(dmatrix& z);
  prnstream& operator<<(d3_array& z);

  int operator ! (void) { return(std::ofstream::operator ! ());}

};

typedef prnstream csvstream;

inline prnstream& prnstream::operator<< (prnstream& (*_f)(prnstream &))
					{ return (*_f)(*this); }

prnstream& endl(prnstream& s);

#endif // #ifndef PRN_T
