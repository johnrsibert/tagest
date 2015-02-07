//$Id: imax.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
//#include <values.h>
#include <limits.h>

/*
int max(const ivector& v)
{
  int t = -INT_MAX; //-MAXINT;
  int i1 = v.indexmin();
  int i2 = v.indexmax();
  for (int i=i1; i <= i2; i++)
  {
    if (v(i) > t)
      t = v(i);
  }
  return t;
}

int max(const imatrix& m)
{
  int t = -INT_MAX; //-MAXINT;
  int i1 = m.rowmin();
  int i2 = m.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    int tt = max(m(i));
    if (tt > t)
      t = tt;
  }
  return t;
}
*/
