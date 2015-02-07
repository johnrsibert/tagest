//$Id: wrap_num.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "wrap_num.h"

int Wrapping_num::add(double increment)
{
  value += increment;
  return wrap();
}

int Wrapping_num::wrap()
{
  int tmp = 0;
  if (value > max)
  {
    tmp = 1;
    double temp = fmod(value + span - min, span);
    value = temp + min;
  }
  else if (value < min)
  {
    tmp = -1;
    double temp = fmod(value + span - min, span);
    value = temp + min;
  }

  last_wrap = tmp;
  return tmp;
}

Wrapping_num::Wrapping_num()
{
  value = 0.0;
  min = 0.0;
  max = 0.0;
  span = 0.0;
  last_wrap = 0;
}

Wrapping_num::Wrapping_num(double _min, double _max, double _value)
{
 set(_min, _max, _value);
}

void Wrapping_num::set(double _min, double _max, double _value)
{
  if (_value < _min)
  {
    cerr << "Warning value, " << _value
	 << ", in Wrapping_num constructor < minimum,"
	 << _min << endl;
    cerr << "value set to " << _min;
    value = _min;
  }

  else if (_value > _max)
  {
    cerr << "Warning value, " << _value
	 << ", in Wrapping_num constructor > maximum,"
	 << _max << endl;
    cerr << "value set to " << _max;
    value = _max;
  }
  else
  {
    value = _value;
  }
  min = _min;
  max = _max;
  span = max-min+1;
  last_wrap = 0;
}

Wrapping_num Wrapping_num::operator + (double t)
{
  Wrapping_num w(min, max, value);
  w.add(t);
  return(w);
}

Wrapping_num Wrapping_num::operator - (double t)
{
  Wrapping_num w(min, max, value);
  w.add(-t);
  return(w);
}

Wrapping_num Wrapping_num::operator += (double t)
{
  add(t);
  return(*this);
}

Wrapping_num Wrapping_num::operator -= (double t)
{
  add(-t);
  return(*this);
}

Wrapping_num Wrapping_num::operator ++ ()
{
  add(1);
  return(*this);
}

Wrapping_num Wrapping_num::operator -- ()
{
  add(-1);
  return(*this);
}

ostream& operator << (ostream& ostr, const Wrapping_num& t)
{
  ostr << t.value;
  return(ostr);
}

int Wrapping_num::operator == (Wrapping_num t) const
{
   if (value == t.value)
     return 1;
   else
     return 0;
}

int Wrapping_num::operator > (double d)
{
  if (value > d)
    return 1;
  else
    return 0;
} 

int Wrapping_num::operator < (double d)
{
  if (value < d)
    return 1;
  else
    return 0;
} 

Wrapping_num Wrapping_num::operator += (Wrapping_num t)
{
  return (*this += t.get_value());
}

Wrapping_num Wrapping_num::operator -= (Wrapping_num t)
{
  return (*this -= t.get_value());
}
