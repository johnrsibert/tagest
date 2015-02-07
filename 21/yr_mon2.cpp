//#include "wrap_int.h"
#include "yr_mon2.h"
//#include "prn_t.h"
#include <stdio.h>
#include <string.h>
//#if defined(sun) || defined(__GNUC__)
#ifdef unix
  #include <strstream>
#else
  #include <strstrea.h>
#endif

#ifndef __ZTC__
  #include <iomanip.h>
#else
  #include <iomanip.hpp>
#endif

char year_month::delimiter = '/';
Wrapping_num year_month::one_month(1, 12, 1);

char set_year_month_delimiter(const char c)
{
  char old_delimiter = year_month::delimiter;
  year_month::delimiter=c;
  return(old_delimiter);
}

year_month::year_month(int _year, int _month)
  : month(1,12,1),
    year(_year)
{
  //year += _month / 12;
  //month += (_month-1);
  set(_year, _month);
}

year_month::year_month(void)
  : month(1,12,1),
    year(0)
{
}

void year_month::set(int _year, int _month)
{
  year = _year + (_month-1) / 12;
//  if (year <= 1900)
//    year += 1900;
  month.set(1,12,1);
  month += (_month-1);
}


int year_month::operator == (const year_month& t)
{
  int month_equal = (month == t.get_month());

  if (!month_equal)
    return(month_equal);

  return (year == t.year);
}

int year_month::operator <= (const year_month& t)
{
  //easy case if host year < arg year
  if ( year < t.year )
    return ( year < t.year );
  else if ( year == t.year ) //compare months if years are the same
    return (month.get_value() <= t.get_month_value());
  else  //host > object t, so return false
    return (0);
}

int year_month::operator < (const year_month& t)
{
  //easy case if host year < arg t.year
  if ( year < t.year )
    return ( year < t.year );
  else if ( year == t.year ) //compare months if years are the same
    return (month.get_value() < t.get_month_value());
  else  //host >= object t, so return false
    return (0);
}

int year_month::operator > (const year_month& t)
{
  if (year > t.year)
    return (year > t.year);
  else if (year == t.year)
    return(month.get_value() > t.month.get_value());
  else
    return (0);
}

int year_month::operator >= (const year_month& t)
{
  if (year > t.year)
    return (year > t.year);
  else if (year == t.year)
    return(month.get_value() >= t.month.get_value());
  else
    return (0);
}

year_month year_month::operator = (const year_month& t)
{
  year = t.year;
  month = t.month;
  return(*this);
}

int year_month::add(Wrapping_num& t)
{
  year += int(t.get_value()) / 12;

  month += t;
  int last_wrap = month.get_last_wrap();
  if (last_wrap > 0)
    year ++;
  else if (last_wrap < 0)
    year --;
  return(last_wrap);
}

year_month year_month::operator += (int t)
{
  year_month y(0,t);
  year += y.get_year();
  //*this += y.get_month();
  month += y.get_month();
  return(*this);
}

int year_month::subtract(Wrapping_num& t)
{
  year -= int(t.get_value()) / 12;

  month -= t;
  int last_wrap = month.get_last_wrap();
  if (last_wrap > 0)
    year ++;
  else if (last_wrap < 0)
    year --;
  return(last_wrap);
}

year_month year_month::operator ++ (void)
{
  add(one_month);
  return(*this);
}

year_month year_month::operator -- (void)
{
  subtract(one_month);
  return(*this);
}

year_month year_month::operator += (Wrapping_num& t)
{
  add(t);
  return(*this);
}

year_month year_month::operator -= (Wrapping_num& t)
{
  subtract(t);
  return(*this);
}

year_month year_month::operator + (Wrapping_num& t)
{
  year_month y(year, month.get_value());
  y.add(t);
  return(y);
}

year_month year_month::operator + (int t)
{
  year_month temp = *this;
  int C = 0;
  while (C < t)
  {
    temp++;
    C++;
  }
  return temp;
}

year_month year_month::operator - (Wrapping_num& t)
{
  year_month y(year,month.get_value());
  y.subtract(t);
  return(y);
}

year_month year_month::operator - (int t)
{
  year_month temp = *this;
  while (t > 0)
  {
    temp--;
    t--;
  }
  return temp;
}

istream& operator>>(istream& istr, year_month& t)
{
  char buf[31]; //year(4) delimiter(1) month(2) null(1)
  istr >> buf;
  if (istr)
  {
    int l = strlen(buf);

    for (int i = 0; i < l; i++)
    {
      if (buf[i] == t.delimiter)
      {
        buf[i] = ' ';
        if (buf[i+1] == '0')
        {
          buf[i+1] = ' ';
        }
      }
    }

    buf[l+1] = '\0';

    std::istrstream ss(buf,l);
    int yr = 0;
    int mon = 0;
    ss >> yr >> mon;

    if (!ss)
    {
      cerr << "error in istrstream" << endl;
      cerr << "buf = '" << buf  << "'" << endl;
      cerr << "l = " << l << endl;
    }

    if ((mon < 1) || (mon > 12) )
    {
      cerr << "month value out of range in istream& operator>>(istream& istr, year_month& t)\n"
           << "valid values are between 1 and 12; you had " << mon << endl;
      cerr << "buf = '" << buf  << "'" << endl;
      cerr << "l = " << l << endl;
      //exit(1);
    #ifdef __BCPLUSPLUS__
      istr.setstate(ios_base::failbit);
    #else
      #if (__GNUC__ >=3)
        istr.setstate(std::ios_base::failbit);
      #else
        istr.set(ios::failbit);
      #endif
    #endif
    }

    t.set(yr,mon);
  }
  return(istr);
}

void year_month::to_string(char* buf)
{
  char tmp[31];
  std::ostrstream s(tmp,30);
  s << year << delimiter << setw(2) << setfill('0') << month.get_value() << '\0';
  strcpy(buf,tmp);
}



ostream& operator<<(ostream& ostr, const year_month& t)
{
  char buf[31];
  t.to_string(buf);
  ostr << buf;
  return(ostr);
}

/*
prnstream& operator<<(prnstream& pstr, year_month& t)
{
  char buf[31];
  t.to_string(buf);
  pstr << buff;
  return(pstr);
}
*/

int operator - (year_month& t2, year_month& t1)
{
#ifdef DOS386
  int diff = 0x7fffffffL;
#else
  int diff = 0x7fff;
#endif
  if ((t1.year > 0) && (t2.year > 0))
  {
    diff = (t2.year - t1.year)*12;
    int tmp = t2.get_month_value() - t1.get_month_value();
    diff += tmp;
  }
  return(diff);
}

#undef YREAR_MONTH_TEST_CODE
#ifdef YREAR_MONTH_TEST_CODE
// stuff for testing classes
int main(void)
{

  /*
  wrapping_int w(1,12,1);

  int i = w.get_value();

  i = w.get_value()+5;

  cout << w << endl;

  for (i=1; i <= 20; i++)
  {
    w++;
    cout << i << "  " << w << endl;
  }


  for (i=1; i <= 20; i++)
  {
    w -= 3;
    cout << i << "  " << w << endl;
  }

  year_month ym(77,5);
  year_month test(77,5);
  ym -= w;
  for (i=1; i <= 20; i++)
  {
     ym += w;
     cout << ym;
     if (ym == test)
       cout << " TRUE " << test;
     else
       cout << " FALSE";

     cout << endl;
  }

  for (i=1; i <= 22; i++)
  {
     ym -= w;
     cout << ym;
     if (ym == test)
       cout << " TRUE " << test;
     else
       cout << " FALSE";

     cout << endl;
  }
  
  year_month d;

  while (1)
  {
    cout << "Enter a year_month: ";
    cin >> d;

    cout << d.get_year() << "  " << d.get_month_value() << endl;
  }
  */


  char old_d = set_year_month_delimiter(':');
  cout << "previous delimiter was '" << old_d << "'" << endl;

  year_month d;
  year_month p;

  long_lat x, y;

  double eff, cat;

  ifstream file("test_eff.dat");
  while ( (file) && (d.get_year() < 84) )
  {
    p = d;
    file >> d >> y >> x >> eff >> cat;
    cout << d << "  " << d.get_year() << "  " << d.get_month_value() << "  "
         << y.value() << "  " << x.value() << "  " << eff << "  " << cat << endl;
  }
  old_d = set_year_month_delimiter('/');
  cout << "previous delimiter was '" << old_d << "'" << endl;
  cout << d << endl;
}
#endif
