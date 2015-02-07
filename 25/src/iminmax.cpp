//$Id: iminmax.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include <limits.h>

int min(const ivector& v)
{
  int t = INT_MAX;
  int i1 = v.indexmin();
  int i2 = v.indexmax();
  for (int i=i1; i <= i2; i++)
  {
    if (v(i) < t)
      t = v(i);
  }
  return t;
}

int min(const imatrix& m)
{
  int t = INT_MAX;
  int i1 = m.rowmin();
  int i2 = m.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    int tt = min(m(i));
    if (tt < t)
      t = tt;
  }
  return t;
}




int max(const ivector& v)
{
  int t = INT_MIN;
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
  int t = INT_MIN;
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
