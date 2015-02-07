#include <fvar.hpp>
#include <values.h>

//find min of an dmatrix
double min(const dmatrix& m)
{
  double t = MAXDOUBLE;
  int i1 = m.rowmin();
  int i2 = m.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    double tt = min(m(i));
    if (tt < t)
      t = tt;
  }
  return t;
}

/*
//find max of an dmatrix
double max(const dmatrix& m)
{
  double t = -MAXDOUBLE;
  int i1 = m.rowmin();
  int i2 = m.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    double tt = max(m(i));
    if (tt > t)
      t = tt;
  }
  return t;
}
*/

