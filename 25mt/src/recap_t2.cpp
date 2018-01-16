//$Id: recap_t2.cpp 2842 2011-08-15 22:35:14Z jsibert $
//#include <iostream>
//#include <iomanip.h>
#include "recap_t.h"
#include "modlarea.h"


int operator == (recaptype& a, recaptype& b)
{
  return(!recaptype_compare(a,b));
}

recaptype& operator += (recaptype& a, recaptype& b)
{
  a.returns += b.returns;
  return(a);
}


int recaptype_compare(const void* _a, const void* _b)
{
  recaptype a = *(recaptype*)_a;
  recaptype b = *(recaptype*)_b;
  int cmp = recaptype_compare(a, b);
  return(cmp);
}

int recaptype_compare(recaptype& a, recaptype& b)
{
  if  (a.date < b.date)
    return(-1);
  else if  (a.date > b.date)
    return(1);
  else
  {
    int cmp = strcmp(a.fleet, b.fleet);
    if (cmp)
      return(cmp);
  }
  if  (a.cohort < b.cohort)
    return (-1);
  else if  (a.cohort > b.cohort)
    return(1);
  else if  (a.i < b.i)
    return (-1);
  else if  (a.i > b.i)
    return(1);
  else if  (a.j < b.j)
    return (-1);
  else if  (a.j > b.j)
    return(1);
  else
    return(0);
}

void recaptype_sort(recaptype_vector& v, const int n)
{
  cout << "qsorting " << n  << " full recaptype_ records" << endl;
  qsort((void *)v.base(), n, sizeof(recaptype), recaptype_compare);
}



