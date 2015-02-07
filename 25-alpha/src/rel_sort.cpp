//$Id: rel_sort.cpp 3100 2013-08-17 01:52:57Z jsibert $
#include "par_t.h"

int tagreleasetype_compare(const void* _a, const void* _b)
{
  tagreleasetype a = *(tagreleasetype*)_a;
  tagreleasetype b = *(tagreleasetype*)_b;
  int cmp = tagreleasetype_compare(a, b);
  return(cmp);
}

int tagreleasetype_compare(tagreleasetype& a, tagreleasetype& b)
{
  if  (a.date < b.date)
    return(-1);
  else if  (a.date > b.date)
    return(1);
  else
    return(0);
}

void tagreleasetype_sort(trv_ptr& tr, const int n)
{
  cout << "qsorting " << n  << " full tagreleasetype_ records" << endl;
  qsort((void *)tr.ptr->base(), n, sizeof(tagreleasetype), tagreleasetype_compare);
}

