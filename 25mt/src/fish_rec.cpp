//$Id: fish_rec.cpp 2981 2012-06-13 18:26:39Z jsibert $
#include "fish_rec.h"
#include "trace.h"
#include <strstream>

/** Logical compare for objects of type fishery_record.
Used for sorting. Arguements are void* and cast to
fishery_recodd*.
\param _a void* points to fishery record a
\param _b void* points to fishery record b
\return int with value -1, 0, or 1 depending if a < b, a == b, or a > b.
*/
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
  else if (a.Lat < b.Lat)
    return(-1);
  else if (a.Lat > b.Lat)
    return(1);
  else if (a.Long < b.Long)
    return(-1);
  else if (a.Long > b.Long)
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
  adstring Gear, Flag; // place holders
/*
old format:
90/01 00N 170E 22.7741       99.9
90/01 00N 173E 41.2258       99.9

01234567890123456789012345678901234567890123456789
current format (with tabs):
2005/01	13.5S	171.0E	0	0	PS	US
2005/01	13.5S	171.5W	0	0	PS	US
2005/01	12.5S	167.5E	1	124.9	PS	US
2005/01	13.0S	168.5E	0	0	PS	US
2005/01	13.0S	169.0E	3.1	42.5	PS	US
2005/01	13.0S	169.5E	0	0	PS	US
*/
  istr.getline(input_record,256);
  istrstream istrstr(input_record);
  istrstr >> Date >> Lat >> Long >> eff >> ctch;
  istrstr >> Gear >> Flag;
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
  strcpy(input_record, t.input_record);

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
  //ostr << t.Fleet << " " << t.Date << " " << t.Lat.value() << " " << t.Long.value()
  ostr << t.Fleet << " " << t.Date << " " << t.get_latitude() << " " << t.get_longitude()
		 << " " << t.eff << " " << t.ctch;
  return(ostr);
}


#include "arrays.cpp"
implement(vector, fishery_record_);

void sort(fishery_record_vector& v, const int n)
{
  qsort((void *)v.base(), (size_t)(n-1), sizeof(fishery_record_), fishery_record_compare);
}
