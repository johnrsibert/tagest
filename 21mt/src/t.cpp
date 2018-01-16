#include <iostream>
#include <iomanip>
#include <math.h>

double gammln(const double xx)
{
   // static double cof[6]={76.18009173,-86.50532033,24.01409822,
   // -1.231739516,0.120858003e-2,-0.536382e-5};
  static double cof[6]={76.18009172947146,-86.50532032941677,
                        24.01409824083091,-1.231739572450155,
                        0.1208650973866179e-2,-0.5395239384953e-5};

  double x=xx-1.0;
  double tmp=x+5.5;
  double tmp1 = tmp-(x+0.5)*log(tmp);
  //double ser=1.0;
  double ser=1.000000000190015;
  for (int j=0;j<=5;j++)
  {
    x++;
    ser += cof[j]/x;
  }

  double z = -tmp1+log(2.50662827465*ser);
  return(z);
}

int main(void)
{
  double x, y;
  x = 100;
  while (x>0)
  {
    cout << "Enter a small number: ";
    cin >> x;
    y = gammln(x+1);
    cout << "gammln(" << x << "+1) = " << y << endl;
    cout << "exp(y) = " << setw(20) << setprecision(10) << exp(y) << endl;
  }
  exit(0);
}

  


