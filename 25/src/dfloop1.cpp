//$Id: dfloop1.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>


//extern "C" void dfloop1(register double* v, register double* dfv, register double* bet, 
//             register double* dfbet, register double* w, register double* dfw, 
//             register double* a, register double* dfa, register double* dfr,
//	     int nl, int nu)
extern "C" void dfloop1(dvector& v, dvector& dfv, dvector& bet, dvector& dfbet, 
             dvector& w, dvector& dfw, dvector& a, dvector& dfa, dvector& dfr,
             int nl, int nu)
{
  register int j;

  for (j = nu; j >= (nl+1); j--)
//  for (j = nu; j >= nl; j--)
  {
     dfw[j]=dfv[j]*bet[j];   //1.
     dfbet[j]+=dfv[j]*w[j];  //2.
     dfv[j] = 0.0;

     dfv[j-1]-=dfw[j]*a[j];  //3.
     dfa[j]-=dfw[j]*v[j-1];  //4.
     dfr[j]+=dfw[j];         //5.
     dfw[j] = 0.0;
  }
}
