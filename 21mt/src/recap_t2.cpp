#include <iostream>
#include <iomanip>
#include "recap_t.h"
#include "modlarea.h"

int gr_recaptype_compare(recaptype& a, recaptype& b);

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

// -----------------------------------------

int gr_recaptype_compare(const void* _a, const void* _b)
{
  recaptype a = *(recaptype*)_a;
  recaptype b = *(recaptype*)_b;
  int cmp = gr_recaptype_compare(a, b);
  return(cmp);
}

int gr_recaptype_compare(recaptype& a, recaptype& b)
{
  if  (a.cohort < b.cohort)
    return (-1);
  else if  (a.cohort > b.cohort)
    return(1);
  if  (a.date < b.date)
    return(-1);
  else if  (a.date > b.date)
    return(1);
  if (a.flindex < b.flindex)
    return(-1);
  else if (a.flindex < b.flindex)
    return(1);
  
 /*
  else
  {
    int cmp = strcmp(a.fleet, b.fleet);
    if (cmp)
      return(cmp);
  }
  */
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

void gr_recap_sort(recaptype_vector& v, const int n)
{
  cout << "qsorting " << n  << " full recaptype_ records" << endl;
  qsort((void *)v.base(), n, sizeof(recaptype), gr_recaptype_compare);
}



