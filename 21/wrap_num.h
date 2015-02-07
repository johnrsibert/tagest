#ifndef __WRAPPING_NUM_H__
#define __WRAPPING_NUM_H__
#include <iostream.h>
#include <math.h>

//const double MAX_RANGE = 360.0;
//const double MIN_RANGE = 0.0;

class Wrapping_num
{
    friend ostream& operator << (ostream& ostr, const Wrapping_num& t);
  public:
    Wrapping_num();
    Wrapping_num(double _min, double _max, double _value);
    virtual ~Wrapping_num() { }

    void set(double _min, double _max, double _value);
    Wrapping_num operator + (double);
    Wrapping_num operator - (double);
    Wrapping_num operator ++ ();
    Wrapping_num operator -- ();
    Wrapping_num operator += (double);
    Wrapping_num operator -= (double);

    Wrapping_num operator += (Wrapping_num);
    Wrapping_num operator -= (Wrapping_num);

    int operator == (Wrapping_num) const;
    int operator > (double);
    int operator < (double);

    int wrap();
    double get_min() { return(min); }
    double get_max() { return(max); }
    double get_value() const { return(value); }
    int get_last_wrap() { return(last_wrap); }

  protected:
    double value;
    double min, max;
    double span;
    int last_wrap;

    int add(double);
};
#endif // __WRAPPING_NUM_H__//
