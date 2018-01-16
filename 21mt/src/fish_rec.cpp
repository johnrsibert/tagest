#include "fish_rec.h"

#if defined(__SUN__) || defined(__GNU__)
int fishery_record_compare(const void* _a, const void* _b)
#else
int cdecl fishery_record_compare(const void* _a, const void* _b)
#endif
{
  fishery_record a = *(fishery_record*)_a;
  fishery_record b = *(fishery_record*)_b;

  int cmp = 0;
  cmp = strcmp(a.Fleet,b.Fleet);
  if (cmp)
    return(cmp);

  if  (a.Date < b.Date)
    return(-1);
  else if  (a.Date > b.Date)
    return(1);
  /*
  else
  {
    int cmp = strcmp(a.Fleet,b.Fleet);
    if (cmp)
      return(cmp);
  }
  */
  else if (a.Long < b.Long)
    return(-1);
  else if (a.Long > b.Long)
    return(1);
  else if (a.Lat < b.Lat)
    return(-1);
  else if (a.Lat > b.Lat)
    return(1);

  else
    return(0);
}

int fishery_record::operator == (fishery_record& t)
{
   int cmp = fishery_record_compare(this, &t);
   return(cmp);
}

istream& fishery_record::read_info(istream& istr)
{
/*
90/01 00N 170E 22.7741       99.9
90/01 00N 173E 41.2258       99.9
*/
  //double ctch;
  istr >> Date >> Lat >> Long >> eff >> ctch;
  //recaps = 0.0;
  return(istr);
}

void fishery_record::set_fleet(const char* t)
{
  const int max_n = 5;
  strncpy(Fleet, t, max_n);
  Fleet[max_n-1] = '\0';
}

fishery_record& fishery_record::operator = (fishery_record& t)
{
  strcpy(Fleet,t.Fleet);
  Date = t.Date;
  Lat = t.Lat;
  Long = t.Long;
  eff = t.eff;
  ctch = t.ctch;
  //recaps = t.recaps;
  return(*this);
}

istream& operator>>(istream& istr, fishery_record& t)
{
  istr >> t.Fleet >> t.Date >> t.Lat >> t.Long >> t.eff
       >> t.ctch;
  return(istr);
}

ostream& operator<<(ostream& ostr, fishery_record& t)
{
  //double ctch = 0.0;
  ostr << t.Fleet << " " << t.Date << " " << t.Lat.value() << " " << t.Long.value()
		 << " " << t.eff << " " << t.ctch;
  //t.recaps = 0.0;
  return(ostr);
}


#include "arrays.cpp"
implement(vector, fishery_record_);

void sort(fishery_record_vector& v, const int n)
{
  qsort((void *)v.base(), n, sizeof(fishery_record_), fishery_record_compare);
}
