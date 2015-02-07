#include <iostream.h>
#include <math.h>
//#include "trace.h"

double dmin(double,double);
double dmax(double, double);

const double ee=3;
const double ee1=ee-1;
const double pp=1e5;

double boundpin(double x, double fmin, double fmax)
{
  double tinv;

  if (x < fmin)
  {
    cout << "Variable out of bounds in boundpin: variable = " <<  x;
    cout <<  "; min = " << fmin;
    cout << "; max = " << fmax << endl;

    x=dmin(fmin+.001,fmin+.01*(fmax-fmin));
  }

  if (x > fmax)
  {
    cout << "Variable out of bounds in boundpin: variable = " <<  x;
    cout <<  "; min = " << fmin;
    cout << "; max = " << fmax << endl;

    x=dmax(fmax-.001,fmax-.01*(fmax-fmin));
  }

  //t=fmin + (fmax-fmin)*(sin(x*1.570795)+1)/2;
  tinv=::asin(2.*(x-fmin)/(fmax-fmin)-1.)/1.570795;
  //tinv=asin(2.*(x-fmin)/(fmax-fmin)-1.)/M_PI_2;
  return(tinv);
}

double dmin(double x, double y)
{
  if (x<y)
  {
    return (x);
  }
  else
  {
    return(y);
  }
}

double dmax(double x, double y)
{
  if (x>y)
  {
    return (x);
  }
  else
  {
    return(y);
  }
}

double boundp( double x, double fmin, double fmax,double& fpen)
{
  double t=fmin + (fmax-fmin)*(sin(x*1.570795)+1)/2;
  //double t=fmin + 0.5*(fmax-fmin)*(sin(x*M_PI_2)+1);

  if (x < -.9999)
  {
    fpen+=pp*pow(-0.9999-x,ee);
  }

  else if (x > 0.9999)
  {
    fpen+=pp*pow(x-0.9999,ee);
  }

  return(t);
}

double dfboundp( double x, double fmin, double fmax, double& dfpen)
{
  dfpen = 0.0;

  if (x < -.9999)
  {
    //fpen+=pp*pow(-0.9999-x,ee);
    dfpen+=pp*ee*pow(-0.9999-x,ee1);
  }

  else if (x > 0.9999)
  {
    //fpen+=pp*pow(x-0.9999,ee);
    dfpen-=pp*ee*pow(x-0.9999,ee1);
  }

  //double t=fmin + (fmax-fmin)*(sin(x*1.570795)+1)/2;
  double dft = 1.570795*(fmax-fmin)*(cos(x*1.570795))/2;
  //double dft = M_PI_4*(fmax-fmin)*cos(x*M_PI_2);
  return(dft);
}
