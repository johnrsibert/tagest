#include "par_t.h"
#include "recap_t.h"

int recap_record_compare(const void* _a, const void* _b)
{
  recaptype a = *(recaptype*)_a;
  recaptype b = *(recaptype*)_b;
 
  int cmp = 0;
 
  if  (a.date < b.date)
    cmp = -1;
  else if  (a.date > b.date)
    cmp = 1;
  else  //equal dates//
    cmp = 0;
 
  return(cmp);
}

void rec_sort(recaptype_vector& v, const int n)
{
  cout << "qsorting " << n  << " recapture records" << endl;
  qsort((void *)v.base(), n, sizeof(recaptype), recap_record_compare);
}

