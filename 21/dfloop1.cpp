#include "precomp.h"

#undef DVECTOR
#define DVECTOR dvector&


//extern "C" void dfloop1(register double* v, register double* dfv, register double* bet, 
//             register double* dfbet, register double* w, register double* dfw, 
//             register double* a, register double* dfa, register double* dfr,
//	     int nl, int nu)
extern "C" void dfloop1(DVECTOR v, DVECTOR dfv, DVECTOR bet, DVECTOR dfbet, 
             DVECTOR w, DVECTOR dfw, DVECTOR a, DVECTOR dfa, DVECTOR dfr,
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
