//$Id: prnstrem.cpp 3208 2014-08-30 06:54:21Z jsibert $
#include "prnstrem.h"
#include <sstream>
#include <string.h>
#include <adstring.hpp>
#include <fvar.hpp>

prnstream::prnstream(const char* fn)
 : std::ofstream(fn)
{ 
  count = 0;
}

prnstream& prnstream::put(char c)
{
  this->std::ostream::put(c);
  if (c == '\n')
    count = 0;
  else
    count++;
  return *this;
}

prnstream& prnstream::operator << (const char* x)
{
  outstr(x,'"');
  return *this;
}

prnstream& prnstream::operator << (const long l)
{
  std::ostringstream s;
  s << l << '\0';
  outstr(s);
  return *this;
}

prnstream& prnstream::operator << (const short i)
{
  return *this << (long)i;
}

prnstream& prnstream::operator << (const int i)
{
  return *this << (long)i;
}

prnstream& prnstream::operator << (const double d)
{
  std::ostringstream s;
  s << d << '\0';
  outstr(s);
  return *this;
}

prnstream& prnstream::operator << (const float f)
{
  return *this << (double)f;
}

prnstream& prnstream::operator << (const dvariable& d)
{
   return *this << value(d);
}

prnstream& prnstream::operator << (const prevariable& d)
{
   return *this << value(d);
}


void prnstream::outstr(const std::ostringstream& os, const char q)
{
  const char* x = os.str().c_str();
  outstr(x,q);
}

void prnstream::outstr(const char* x, const char q)
{
  int n = strlen(x) -1;

  if (strcmp(x, "\n") == 0)
  {
    put(x[0]);
    return;
  }

  if (count > 0)
    put(',');
  if (q != '\0')
    put(q);
  for (int i = 0; i < n; i++)
    put(x[i]);

  if (x[n] != '\n')
  {
    put(x[n]);
    if (q != '\0')
      put(q);
  }
  else
  {
    if (q != '\0')
      put(q);
    put('\n');
  }
  return;
}

prnstream& endl(prnstream& s)
{
  s << "\n";
  s.flush();
  return s;
}

prnstream& prnstream::operator << (const adstring& s)
{
  return *this << (const char*)s;
}

#undef PRNSTREAM_TEST_CODE
#ifdef PRNSTREAM_TEST_CODE
int main(void)
{
  adstring ads("This is an ADSTRING");
  prnstream prn("test.prn");
  double x = 1.2345;

  prn << x <<  x << "string" << ads << endl;

  csvstream csv("test.csv");
  csv << x << "csv string" << x << ads << endl;
}
#endif


