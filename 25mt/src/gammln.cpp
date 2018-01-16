//$Id: gammln.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include <math.h>

template <typename DOUBLE>
DOUBLE mm_gammln(const DOUBLE xx)
{
  static double cof[6]={76.18009172947146,-86.50532032941677,
                        24.01409824083091,-1.231739572450155,
                        0.1208650973866179e-2,-0.5395239384953e-5};

  DOUBLE x=xx-1.0;
  DOUBLE tmp=x+5.5;
  DOUBLE tmp1 = tmp-(x+0.5)*log(tmp);
  DOUBLE ser=1.000000000190015;
  for (int j=0;j<=5;j++)
  {
    x+=1;
    ser += cof[j]/x;
  }

  DOUBLE z = -tmp1+log(2.50662827465*ser);
  return(z);
}
template double mm_gammln(const double xx);
template dvariable mm_gammln(const dvariable xx);
