//$Id: dfsfabs.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <math.h>

 double mm_sfabs(double v1)
 {
   #define cutoff  1.0
   #define cutoff2 1.0
   double a = 2.;// /cutoff;
   double b = 1.;// /cutoff2;

   double tmp,tmp1,t2;
   double return_value;
   tmp1=v1;
   tmp=::fabs(tmp1);
   if (tmp > cutoff)
   {
	return_value=tmp;
   }
   else
   {
     t2=tmp*tmp;

     if (tmp1 >=0)
     {
       return_value=t2*(a-b*tmp1);
     }
     else
     {
       return_value=t2*(a+b*tmp1);
     }
   }
   return(return_value);
 }


 double mm_dfsfabs(double v1)
 {
   #define cutoff 1.0
   #define cutoff2 1.0
   double a = 2.;// /cutoff;
   double b = 1.;// /cutoff2;

   double tmp,tmp1,t2;
   double return_value;
   tmp1=v1;
   tmp=::fabs(tmp1);
   if (tmp > cutoff)
   {
     if (tmp1 > 0)
     {
       return_value=1.0;
     }
     else
     {
       return_value=-1.0;
     }
   }
   else
   {
     t2=tmp*tmp;

     if (tmp1 >=0)
     {
       //return_value=t2*(a-b*tmp1);
       //return_value=v1*v1*(a-b*v1);
       return_value=v1*(2.*a-3.*b*v1);
     }
     else
     {
       //return_value=t2*(a+b*tmp1);
       //return_value=v1*v1*(a+b*v1);
       return_value=v1*(2.*a+3.*b*v1);
     }
   }
   //TRACE(v1)
   //TRACE(return_value)
   return(return_value);
 }

