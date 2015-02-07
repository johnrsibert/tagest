//$Id: baranov.cpp 3127 2013-12-05 22:40:36Z jsibert $
#include "par_t.h"
#include "trace.h"

 
dvariable par_t::baranov_catch_f0(const dvariable& f1, const dvariable& f0, 
                                  const dvariable& P, const dvariable& rr)
{
  dvariable z = mort + f1 + f0;
  dvariable c = rr*P*f0/z*(1.0-mfexp(-z));
  return(c); 
}
 
dvariable par_t::dbaranov_catch_df0(const dvariable& f1, const dvariable& f0, 
                                    const dvariable& P, const dvariable& rr)
{
  dvariable z = mort + f1 + f0 + 1e-8;
  dvariable expz = mfexp(-z);
  //dvariable dGdF0 = - f0*expz*rr*P/z - (1.0-expz)*rr*P/z + f0*(1-expz)*rr*z/square(z);
  dvariable dGdF0 = rr*P*((1.0/z-f0/square(z))*(1-expz) + (f0/z)*expz);
  return(dGdF0);
}

// baranov.R
// D(g,"F")
// (1/(F + M) - F/(F + M)^2) * (1 - exp(-F - M)) + (F/(F + M)) * exp(-F - M)


#undef NR_DEBUG
 
dvariable par_t::
NR_baranov_f0(const double& C, const dvariable& f1, const dvariable& P, const dvariable& rr, const dvariable& init_f0)
{ 
  ASSERT((f1 <= 0.0))
  if (C > rr*sfabs(P))
    return(init_f0);
    
  #ifdef NR_DEBUG
  clogf << endl;
  TTRACE(C,P)
  const double eps = 1.0e-3;
  #endif
  const int maxit = 5;
  dvariable f0 = C/(rr*sfabs(P)+1e-8); // initial guess
  dvariable dx = 0.0;
  dvariable predC = 0.0;
  int it = 0;
  for (it = 1; it <= maxit; it++)
  {
    predC = baranov_catch_f0(f1, f0, P, rr);
    dvariable g = C/predC - 1.0;
    dvariable dg = dbaranov_catch_df0(f1, f0, P, rr)+1.0e-8;
    dx = g/dg;
  #ifdef NR_DEBUG
    TTRACE(C,predC)
    TTRACE(g,dg)
    TTRACE(dx,f0)
   #endif
  
    f0 += dx;
  }
  #ifdef NR_DEBUG
  if (fabs(dx) <= eps)
  {
    clogf << "Yes! Newton-Raphson approximation to f0 converged after " 
           << maxit << " iterations; dx = " << dx << endl;
  }
  else
  {
     clogf << "Warning Newton-Raphson approximation to f0 not converged after " 
           << maxit << " iterations; dx = " << dx << endl;
  }
  clogf << "        f0 = " << f0 << ", C = " << C << ", Baranov C = " << predC << ", P = " << P << endl;
  #endif //ifdef NR_DEBUG

  return(f0);
}
/*
  dvariable bound(const dvariable& x, const double& l, const double& h, const double& eps)
  {
    dvariable ret;
    if((x>=(l+eps))&&(x<=(h-eps))){
      ret=x;
    }else{
      if(x<(l+eps)){
        ret=eps*mfexp((x-(l+eps))/eps)+l;
      }else{
        if(x>(h-eps)){
          ret=h-eps*mfexp(((h-eps)-x)/eps);
        }
      }
    }
    return ret;
  }
*/

