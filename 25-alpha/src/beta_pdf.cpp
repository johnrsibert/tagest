//$Id: beta_pdf.cpp 3204 2014-08-18 01:47:44Z jsibert $
/** \file
Source code for beta probability density function.
Based on Steve Martell's dbeta.cpp in the ADMB contributed library statLib.
The code in this file returns the unvarnished beta proability density and NOT
the negative log of the probability density.
\f$
 	f(x; \alpha, \beta) = \frac{\Gamma(\alpha+\beta))}{\Gamma(\alpha)\Gamma(\beta)}x^{\alpha-1}(1-x)^{\beta-1}
 \f$
*/

#include <fvar.hpp>

double beta_pdf(const double& x, const double& a, const double& b);
dvariable beta_pdf(const dvariable& x, const dvariable& a, const dvariable& b);
dvector beta_pdf(const dvector& x, const dvariable& a, const dvariable& b);
dvar_vector beta_pdf(const dvar_vector& x, const dvariable& a, const dvariable& b);

/** beta density for constant objects.
\param x Realization of random variable, \f$x\f$, from the beta distribution.
\param a Beta shape parameter \f$ \alpha\f$
\param b Beta shape parameter \f$ \beta\f$
\returns \f$ f(x; \alpha, \beta) \f$ as a double.
*/
double beta_pdf(const double& x, const double& a, const double& b)
{
   if( x<=0 || x>=1.0 )
   {
      cerr<<"x is <=0 or >=1.0 in "
         "beta_pdf( const double& x, const double& a, const double& b )\n";
      cerr << "x = " << x << endl;
      return 0;
   }
   
   if( a<=0 || b <=0 )
   {
      cerr<<"a or b is <= 0 in "
         "beta_pdf( const double& x, const double& a, const double& b )\n";
      cerr << "a = " << a << "; b = " << b << endl;
      return 0;
   }
   
   double px = exp(gammln(a+b)-gammln(a)-gammln(b)+(a-1.0)*log(x)+(b-1.0)*log(1.0-x));
   return px;
}

/** beta density for a vector of constant objects.
\param x Vector of realizations of random variable, \f$x_i\f$, from the beta distribution.
\param a Beta shape parameter \f$ \alpha\f$
\param b Beta shape parameter \f$ \beta\f$
\returns A dvector of probablities the same length as x.
*/
dvector beta_pdf(const dvector& x, const double& a, const double& b)
{
   const int i1 = x.indexmin();
   const int i2 = x.indexmax();
   //cout << i1 << " " << i2 << endl;
   dvector px(i1,i2);
   for (int i = i1; i <= i2; i++)
      px(i) = beta_pdf(x(i),a,b);
   return px;
}

/** beta density for variable objects.
\param x Realization of random variable, \f$x\f$, from the beta distribution.
\param a Beta shape parameter \f$ \alpha\f$
\param b Beta shape parameter \f$ \beta\f$
\returns \f$ f(x; \alpha, \beta) \fr$ as a dvariable
*/
dvariable beta_pdf(const dvariable& x, const dvariable& a, const dvariable& b)
{
   if( x<=0 || x>=1.0 )
   {
      cerr<<"x is <=0 or >=1.0 in "
         "beta_pdf( const double& x, const double& a, const double& b )\n";
      cerr << "x = " << x << endl;
      return 0;
   }
   
   if( a<=0 || b <=0 )
   {
      cerr<<"a or b is <= 0 in "
         "beta_pdf( const double& x, const double& a, const double& b )\n";
      cerr << "a = " << a << "; b = " << b << endl;
      return 0;
   }
   
   dvariable px = exp(gammln(a+b)-gammln(a)-gammln(b)+(a-1.0)*log(x)+(b-1.0)*log(1.0-x));
   return px;
}

/** beta density for a vector of variable objects.
\param x Vector of realizations of random variable, \f$x_i\f$, from the beta distribution.
\param a Beta shape parameter \f$ \alpha\f$
\param b Beta shape parameter \f$ \beta\f$
\returns A dvar_vector of probablities the same length as x.
*/
dvar_vector beta_pdf(const dvar_vector& x, const dvariable& a, const dvariable& b)
{
   const int i1 = x.indexmin();
   const int i2 = x.indexmax();
   RETURN_ARRAYS_INCREMENT();
   dvar_vector px(i1,i2);
   for (int i = i1; i <= i2; i++)
      px(i) = beta_pdf(x(i),a,b);
   RETURN_ARRAYS_DECREMENT();
   return px;
}

#ifdef TEST_CODE
#include <statsLib.h>
int main(int argc,char * argv[])
{
   gradient_structure gs(1000000);
   gradient_structure::set_NO_DERIVATIVES();

   dvariable a = 1.1;
   dvariable b = 1.1;
   const int x1 = 1;
   const int x2 = 100;
   const double dx = double(x1)/double(x2);
   dvar_vector x(x1,x2);
   dvar_vector px(x1,x2);
   dvar_vector db(x1,x2);
   x.fill_seqadd(dx,dx);
   //cout << x << endl;
   while ( (a > 0.0) && (b > 0.0))
   {
     dvar_vector ppx = beta_pdf(x,a,b);
     dvariable sumpx = 0.0;
     dvariable sumdb = 0.0;
     for (int i = x1; i <= x2 && x(i) < 1.0; i++)
     {
        px(i) = beta_pdf(x(i),a,b);
        db(i) = exp(-dbeta(x(i),a,b));
        cout << i << " " << x(i) << " " << px(i)  << " " << ppx(i) << " " << db(i) << endl;
        sumpx += px(i)*dx;
        sumdb += db(i)*dx;
     }
     cout << " sum " << sumpx << " " <<  dx*sum(ppx) <<  " " << sumdb << endl;
     cout << "Enter a b:";
     cin >> a >> b;
  }
}
#endif // TEST_CODE undefined
