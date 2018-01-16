#include "prnstrem.h"
#include <string.h>
#ifdef unix
#include <strstream>
#else
#include <strstrea.h>
#endif
#if defined(__ZTC__)// && (__ZTC__ <= 0x310)
  #include "adstring.hpp"
#endif


prnstream::prnstream(const char* fn)
#ifdef __BCPLUSPLUS__
 : ofstream(fn)
#endif
#ifdef __SUN__
 : ofstream(fn)
#endif
#ifdef __ZTC__
 : ios(&buffer), ofstream(fn)
#endif
{ 
  count = 0;
}

prnstream& prnstream::put(char c)
{
  this->ostream::put(c);
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
  char b[50];
  std::ostrstream s(b,50);
  s << l << '\0';
  outstr(b);
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
  char b[50];
  std::ostrstream s(b,50);
  s << d << '\0';
  outstr(b);
  return *this;
}

prnstream& prnstream::operator << (const float f)
{
  return *this << (double)f;
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

#if defined(__ZTC__) //&& (__ZTC__ <= 0x310)
prnstream& pendl(prnstream& s)
{
  s << "\n";
  s.flush();
  return s;
}

prnstream& prnstream::operator << (const adstring& s)
{
  return *this << (char*)s;
}

#endif
