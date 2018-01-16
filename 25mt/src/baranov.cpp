//$Id: baranov.cpp 2859 2011-10-03 18:52:55Z jsibert $
#include "par_t.h"
#include "trace.h"

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
baranov_catch_f0(const DOUBLE& f1, const DOUBLE& f0, const DOUBLE& P, const DOUBLE& rr)
{
  DOUBLE z = mort + f1 + f0;
  DOUBLE c = rr*P*f0/z*(1.0-mfexp(-z));
  return(c); 
}
template double par_t<d3_array,dmatrix,dvector,double>::
baranov_catch_f0(const double& f1, const double& f0, const double& P, const double& rr);
template dvariable par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
baranov_catch_f0(const dvariable& f1, const dvariable& f0, const dvariable& P, const dvariable& rr);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
dbaranov_catch_df0(const DOUBLE& f1, const DOUBLE& f0, const DOUBLE& P, const DOUBLE& rr)
{
  DOUBLE z = mort + f1 + f0 + 1e-8;
  DOUBLE expz = mfexp(-z);
  //DOUBLE dGdF0 = - f0*expz*rr*P/z - (1.0-expz)*rr*P/z + f0*(1-expz)*rr*z/square(z);
  DOUBLE dGdF0 = rr*P*((1.0/z-f0/square(z))*(1-expz) + (f0/z)*expz);
  return(dGdF0);
}
template double par_t<d3_array,dmatrix,dvector,double>::
dbaranov_catch_df0(const double& f1, const double& f0, const double& P, const double& rr);
template dvariable par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
dbaranov_catch_df0(const dvariable& f1, const dvariable& f0, const dvariable& P, const dvariable& rr);

// baranov.R
// D(g,"F")
// (1/(F + M) - F/(F + M)^2) * (1 - exp(-F - M)) + (F/(F + M)) * exp(-F - M)


#undef NR_DEBUG
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::
NR_baranov_f0(const double& C, const DOUBLE& f1, const DOUBLE& P, const DOUBLE& rr, const DOUBLE& init_f0)
{ 
  ASSERT((f1 <= 0.0))
  if (C > rr*sfabs(P))
    return(init_f0);
    
  #ifdef NR_DEBUG
  clogf << endl;
  TTRACE(C,P)
  #endif
  const double eps = 1.0e-3;
  const int maxit = 5;
  DOUBLE f0 = C/(rr*sfabs(P)+1e-8); // initial guess
  DOUBLE dx = 0.0;
  DOUBLE predC = 0.0;
  int it = 0;
  for (it = 1; it <= maxit; it++)
  {
    predC = baranov_catch_f0(f1, f0, P, rr);
    DOUBLE g = C/predC - 1.0;
    DOUBLE dg = dbaranov_catch_df0(f1, f0, P, rr)+1.0e-8;
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
template double par_t<d3_array,dmatrix,dvector,double>::
NR_baranov_f0(const double& C, const double& f1, const double& P, const double& rr, const double& init_f0);
template dvariable par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::
NR_baranov_f0(const double& C, const dvariable& f1, const dvariable& P, const dvariable& rr, const dvariable& init_f0);

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

