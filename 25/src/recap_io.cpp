//$Id: recap_io.cpp 3125 2013-11-19 02:44:01Z jsibert $
#include "par_t.h"
#include "recap_t.h"

ostream& operator << (ostream& s, recaptype& t)
{
  ios_base::fmtflags sflags = s.flags();
  int swidth = s.width();
  int sprecision = s.precision();

  s.setf(ios::fixed,ios::floatfield);
  s << "    "  << t.fleet
    << setfill(' ')
    << setw(3) << t.par_flindex
    << setw(3) << t.ifr_flindex
    << setw(12) << t.date
    << setw(6) << t.cohort
    << setw(4) << t.i
    << setw(3) << t.j
    << setw(9) << t.returns;

  s.precision(sprecision);
  s.width(swidth);
  s.flags(sflags);

  return(s);
}

void recaptype::write_on(ostream& s, Model_area& m)
{
  ios_base::fmtflags sflags = s.flags();
  int swidth = s.width();
  int sprecision = s.precision();

  s.setf(ios::fixed,ios::floatfield);
  s << "    "  << fleet
    << setfill(' ')
    << setw(5) << date.get_year()
    << setw(5) << date.get_month_value()
    << setw(6) << cohort
    << setw(8) << setprecision(2) << m.index_to_long(i)
    << setw(7) << setprecision(2) << m.index_to_lat(j)
    << setw(9) << returns;


  s.precision(sprecision);
  s.width(swidth);
  s.flags(sflags);
}

void recaptype::read_from(istream& s, Model_area& m)
{
  int   year;     // month of recapture
  int   month;    // month of recapture
  double lon, lat;
  s >> fleet;
  s >> year;
  s >> month;
  s >> cohort;
  s >> lon;//t.i;
  s >> lat;//t.j;
  s >> returns;
  date.set(year, month);
  i = m.long_to_index(lon);
  j = m.lat_to_index(lat);
}

/*
ostream& operator << (ostream& s, recaptype_& t)
{
  s << setprecision(0)
#ifdef __ZTC__
    << setfixed
#endif
    << "    "  << t.fleet
    << setfill(' ')
    << setw(5) << t.date.get_year()
    << setw(5) << t.date.get_month_value()
    << setw(6) << t.cohort
    << setw(7) << t.i
    << setw(7) << t.j
    << setw(9) << t.returns;

  return(s);
}

istream& operator >> (istream& s, recaptype_& t)
{
  int   year;     // month of recapture
  int   month;    // month of recapture
  s >> t.fleet;
  s >> year;
  s >> month;
  s >> t.cohort;
  s >> t.i;
  s >> t.j;
  s >> t.returns;
  t.date.set(year, month);

  return(s);
}

cifstream& operator >> (cifstream& s, recaptype_& t)
{
  int   year;     // month of recapture
  int   month;    // month of recapture
  s >> t.fleet;
  s >> year;
  s >> month;
  s >> t.cohort;
  s >> t.i;
  s >> t.j;
  s >> t.returns;
  t.date.set(year, month);

  return(s);
}

*/
