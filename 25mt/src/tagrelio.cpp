//$Id: tagrelio.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t.h"
ostream& operator << (ostream& s, tagreleasetype_& t)
{
   s << setw(8) << t.cohort
     << setw(8) <<  t.date.get_year() //month
     << setw(8) << t.date.get_month_value()
     << setw(8) << t.i
     << setw(8) << t.j
     << setw(8) << int(t.tags);
  return(s);
}

//istream& operator >> (istream& s, tagreleasetype_& t);
//cifstream& operator >> (cifstream& s, tagreleasetype_& t);
