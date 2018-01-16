#include <fvar.hpp>
#include <math.h>

double dfgammln(const double v1)
{
  static double cof[6]={76.18009172947146,-86.50532032941677,
                        24.01409824083091,-1.231739572450155,
                        0.1208650973866179e-2,-0.5395239384953e-5};

  dvector x(-1,5);
  dvector dfx(-1,5);
  dfx.initialize();

  int j;
  double tmp=0.0;
  double tmp1=0.0;
  double xx=0.0;
  double dftmp=0.0;
  double dftmp1=0.0;
  double dfser=0.0;
  double dfxx=0.0;
  xx=v1; //value(v1);
  x(-1)=xx-1.0;
  tmp=x(-1)+5.5;
  tmp1 = tmp -(x(-1)+0.5)*log(tmp);
  double ser=1.000000000190015;
  for (j=0;j<=5;j++) 
  {
    x(j) = x(j-1)+1.0;
    ser += cof[j]/x(j);
  }

  double z = -tmp1+log(2.50662827465*ser);

  dftmp1 = -1.;
  dfser = 1.0/ser;

  for (j=5;j>=0;j--) 
  {
    //ser += cof[j]/x(j);
    dfx(j)-=dfser*cof[j]/(x(j)*x(j));
    //x(j) = x(j-1)+1.0;
    dfx(j-1)+=dfx(j);
    dfx(j)=0.;
  }
  //ser=1.0;
  dfser=0.0;
  //tmp1 = tmp - (x(-1)+0.5)*log(tmp);
  dfx(-1)-=dftmp1*log(tmp);
  dftmp += dftmp1*(1.0 -(x(-1)+0.5)/tmp);
  dftmp1=0.0;
  //tmp=x(-1)+5.5;
  dfx(-1)+=dftmp;
  dftmp=0.0;
  //x(-1)=xx-1.0;
  dfxx=dfx(-1);
  dfx(-1)=0.0;
  //x=xx-1.0;
  dfxx+=dfx(-1);
  dfx(-1)=0.0;

  return(dfxx);
}


