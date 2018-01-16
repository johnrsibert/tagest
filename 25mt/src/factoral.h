//$Id: factoral.h 2754 2011-01-02 20:57:07Z jsibert $
#ifndef FACTORIAL_CLASS_H
#define FACTORIAL_CLASS_H
#include <fvar.hpp>

class factorial_class
{
  static int instances;
  int maxn;
  dvector* x;
  dvector* lx;

public:
  factorial_class(const int n);
 ~factorial_class();
  double factorial(const int i);
  double log_factorial(const int i);
};

#endif// FACTORIAL_CLASS_H
