//$Id: liketest.cpp 2842 2011-08-15 22:35:14Z jsibert $
#include <fvar.hpp>
#include <adstring.hpp>
#include <iostream>
#include <fstream.h>
#include <iomanip.h>
#include <strstream.h>
#include "trace.h"
ofstream clogf("liketest.log");

double mm_gammln(const double xx);
void write_like_matrix(const char* root, dmatrix& p, dvector& obs);

int main(void)
{

  cout << "started main" << endl;
  dvector prd("{0,1,2,3,4,5,10,25,50,100}");
  int j1=prd.indexmin();
  int j2=prd.indexmax();
  TTRACE(j1,j2)
  TRACE(prd)

  const int nobs = 100;

  const double eps = 1e-8;
  dmatrix P(0,nobs,j1,j2);

  int i;
  for (i = 0; i <= nobs; i++)
  {
    double obs = i;
    //cout << setw(4) << obs;
    for (int j = j1; j <= j2; j++)
    {
       double tobs = obs+eps;
       double tprd = prd(j)+eps;
       P(i,j) =  tprd - tobs*log(tprd) + mm_gammln(tobs+1);
       P(i,j) = exp(-P(i,j));
       //cout << setw(8) << setprecision(3) << P(i,j);
    }
    //cout << endl;
  }
  //TRACE(P)
  write_like_matrix("poisson", P, prd);
  cout << "Finished Poisson" << endl;
 
  for (i = 0; i <= nobs; i++)
  {
    double obs = i;
    //cout << setw(4) << obs;
    for (int j = j1; j <= j2; j++)
    {
       double tobs = obs+eps;
       double tprd = prd(j)+eps;
       P(i,j) =  tobs/tprd + log(tprd);
       P(i,j) = exp(-P(i,j));
       //cout << setw(8) << setprecision(3) << P(i,j);
    }
    //cout << endl;
  }
  //TRACE(P)
  write_like_matrix("exponential", P, prd);
  cout << "Finished exponential" << endl;

  double a = 0.1;
  cout << "enter a value for a in negative binomial: ";
  cin >> a;
  for (i = 0; i <= nobs; i++)
  {
    double obs = i;
    //cout << setw(4) << obs;
    for (int j = j1; j <= j2; j++)
    {
       double tobs = obs+eps;
       double tprd = prd(j)+eps;
       P(i,j) = mm_gammln(tobs+a) - mm_gammln(a) - mm_gammln(tobs+1.0);
       P(i,j) += a*(log(a) - log(a+tprd)) + tobs*(log(tprd)-log(a+tprd));
       P(i,j) = exp(P(i,j));
       //cout << setw(8) << setprecision(3) << P(i,j);
    }
    //cout << endl;
  }
  //TRACE(P)
  char ss[81];
  strstream fn(ss,80);
  fn << "negative_binomial_a=" << a << ends;
  write_like_matrix(ss, P, prd);
  cout << "Finished Negative Binomial; file: " << ss << endl;

  clogf.close();
  cout << "finished main" << endl;
  return(0);
}  

void write_like_matrix(const char* root, dmatrix& p, dvector& prd)
{
  adstring filename = adstring(root)+ adstring(".dat");
  ofstream pf(filename);
  if (!pf)
  {
     cerr << "Error opening " << filename << endl;
     exit(1);
  }
  int j1=prd.indexmin();
  int j2=prd.indexmax();
  pf << "\"obs\"";
  for (int j = j1; j <= j2; j++)
  {
     pf << "  \"P" << prd(j) << '\"';
  }
  pf  << endl;

  int i1 = p.rowmin();
  int i2 = p.rowmax();
  TTRACE(i1,i2)

  for (int i = i1; i <= i2; i++)
  {
    pf << setw(4) << i;
    for (int j = j1; j <= j2; j++)
    {
       double prob = p(i,j);
       //if (prob < 1e-3)
         //prob = 0.0;
       int prec = 3;
       if (prob > 1e3)
         prec = 3;
       //pf << setw(9) << setprecision(prec) << prob;
       pf << "  " << prob;
    }
    pf << endl;
  }
    
}

