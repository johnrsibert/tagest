//$Id: factorl.cpp 3075 2013-04-05 21:17:39Z jsibert $
#include "factoral.h"

int factorial_class::instances = 0;
//int factorial_class::maxn = 0;
//dvector factorial_class::x;
//dvector factorial_class::lx;


factorial_class::~factorial_class()
{
  //std::cerr << "In  factorial_class::~factorial_class()" << endl;
  x.~dvector();
  lx.~dvector();
  //std::cerr << "Out factorial_class::~factorial_class()" << endl;
}

factorial_class::factorial_class(const int n)
{
  if (instances > 0)
  {
    cerr << "You are only allowed one instance of factorial_class." << endl;
    exit(1);
  }

  maxn = n;
  instances ++;
  x.allocate(0,n);
  lx.allocate(0,n);

  x[0] = 1;
  lx[0] = 0;

  for (int i = 1; i <= n; i++)
  {
    x[i] = x[i-1]*(double)i;
    lx[i] = log( x[i] );
  }
}

double factorial_class::factorial(const int i)
{
  if (i <= maxn)
  {
    return x[i];
  }
  else
  {
    double root_two_pi = 2.5066283;
    double t = double(i);
    double s = (t+0.5)*log(t)-t;
    s = root_two_pi*exp(s);
    return(s);
  }
}

double factorial_class::log_factorial(const int i)
{
  if (i <= maxn)
  {
    return lx[i];
  }
  else
  {
    double log_root_two_pi = 0.9189385;
    double t = double(i);
    double s = log_root_two_pi+(t+0.5)*log(t)-t;
    return(s);
  }
}

