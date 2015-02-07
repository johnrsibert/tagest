//$Id: yr_mon2.h 2842 2011-08-15 22:35:14Z jsibert $
#if (__GNUC__ >=3)
  #include <iostream>
  #include <iomanip>
  using namespace std;
#else
  #include <iostream>
  #include <iomanip.h>
#endif
#include <stdlib.h>

#ifndef __year_month_h__
#define __year_month_h__

#include "wrap_num.h"

class prnstream;

class year_month
{
  protected:
    static Wrapping_num one_month;
    static char delimiter;

    Wrapping_num month;
    int year;

    int add(Wrapping_num&);
    int subtract(Wrapping_num&);
    void to_string(char* s) const;

  public:

    year_month(void);
    year_month(int y, int m);
    ~year_month(void) { }

    void set(int y, int m);

    Wrapping_num get_month(void) { return (month); }
    int get_month_value(void) const { return ((int)month.get_value()); }
    int get_year(void) const { return (year); }

    year_month operator = (const year_month&);

    // increment and decrement by one month
    year_month operator ++ (void);
    year_month operator ++ (int) {return(++*this);}
    year_month operator -- (void);
    year_month operator -- (int) {return(--*this);}

    year_month operator += (Wrapping_num&);
    year_month operator + (Wrapping_num&);
    year_month operator + (int);
    year_month operator -= (Wrapping_num&);
    year_month operator - (Wrapping_num&);
    year_month operator - (int);

    year_month operator += (int);

    bool operator == (const year_month&) const;
    int operator <= (const year_month&);
    int operator >= (const year_month&);
    int operator < (const year_month&);
    int operator > (const year_month&);
    int operator != (const year_month& t) {return( !(*this == t));}

    friend istream& operator>>(istream& istr, year_month& t);
    friend ostream& operator<<(ostream& ostr, const year_month& t);
    friend prnstream& operator<<(prnstream& ostr, const year_month& t);

    friend int operator - (year_month& t1, year_month& t2);

    friend void show_date(int x, int y, year_month& t);
    friend char set_year_month_delimiter(const char c);
};

typedef year_month year_month_;
struct year_month_vrep
{
   year_month_ *p;
   int en1;
   int en2;
   int nsize;
   int refcnt;
};

class year_month_vector
{
   year_month_vrep *v;
   void allocate();
 public: 
   year_month_vector(const int n1, const int n2);
   year_month_vector(){v=NULL;};
   void allocate(const int n1, const int n2);
   year_month_ *base()
   {
      return v->p;
   }
   inline const int indexmin(void) const {return (v->en1);}
   inline const int indexmax(void) const {return (v->en2);}
#ifdef OPT_LIB
  inline year_month_& operator[](const int i) {return v->p[i-v->en1];}
  inline year_month_& operator()(const int i) {return v->p[i-v->en1];}

  inline const year_month_& operator[](const int i) const {return v->p[i-v->en1];}
  inline const year_month_& operator()(const int i) const {return v->p[i-v->en1];}
#else
  year_month_& operator[](const int i);
  year_month_& operator()(const int i);

  const year_month_& operator[](const int i) const;
  const year_month_& operator()(const int i) const;
#endif
   year_month_vector & operator=(year_month_vector & x);
   ~year_month_vector();
   inline year_month_vector(year_month_vector & x)
   {
      x.v->refcnt++;
      v = x.v;
   }

  friend ostream& operator<<(ostream& ostr, const year_month_vector& t);
};

#endif // __year_month_h__
