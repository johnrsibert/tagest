#include <iostream>
#include <stdlib.h>

#ifndef __year_month_h__
#define __year_month_h__

#include <stdio.h>
#include <string.h>
#include <strstream>
using std::istrstream;
using std::ostrstream;
#include <iomanip>
#include "wrap_num.h"

class prnstream;

class year_month
{
  protected:
    static char delimiter;

    int year;
    Wrapping_num month;

    int add(Wrapping_num&);
    int subtract(Wrapping_num&);
    void to_string(char* s);

  public:
    static Wrapping_num one_month;

    year_month(void);
    year_month(int y, int m);
    ~year_month(void) { }

    void set(int y, int m);

    Wrapping_num& get_month(void) { return (month); }
    int get_month_value(void) const { return ((int)(month.get_value())); }
    int get_year(void) { return (year); }

    year_month operator=(const year_month&);

    // increment and decrement by one month
    year_month operator ++ (void);
    year_month operator ++ (int) {return(++*this);}
    year_month operator -- (void);
    year_month operator -- (int) {return(--*this);}

    year_month operator += (Wrapping_num&);
    year_month operator + (Wrapping_num&);
    year_month& operator + (const int);
    year_month operator -= (Wrapping_num&);
    year_month operator - (Wrapping_num&);
    year_month operator - (int);

    year_month operator += (int);

    int operator == (const year_month&);
    int operator <= (year_month&);
    int operator >= (year_month&);
    int operator < (year_month&);
    int operator > (year_month&);
    int operator != (year_month& t) {return( !(*this == t));}

    friend istream& operator>>(istream& istr, year_month& t);
    friend ostream& operator<<(ostream& ostr, year_month& t);
    friend prnstream& operator<<(prnstream& ostr, year_month& t);

    friend int operator - (year_month& t1, year_month& t2);

    friend void show_date(int x, int y, year_month& t);
    friend char set_year_month_delimiter(const char c);
};

#include "arrays.hpp"
//#include "arrays.cpp"
typedef year_month year_month_;
declare(vector, year_month_);

#endif // __year_month_h__
