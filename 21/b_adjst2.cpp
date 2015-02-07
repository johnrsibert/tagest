#include "precomp.h"
#include "precomp2.h"
#include <fstream.h>
#define ELEM elem

adstring effort_file_name(adstring& fleet, year_month& date);

extern ofstream clogf;

void coff_t::b_adjust(par_t& param, dmatrix& z)
{
  dmatrix& acoff = a;
  dmatrix& bcoff = b;
  dmatrix& raw_bcoff = raw_b;
  dmatrix& ccoff = c;

  dmatrix& xbet_c = xbet;
  dmatrix& xgam_c = xgam;

  //ivector& jlb = param.jlb;
  //ivector& jub = param.jub;
  //ivector& ilb = param.ilb;
  //ivector& iub = param.iub;
  //int m     = param.get_m();
  int n     = param.get_n();

  //bcoff = raw_bcoff + trans(z);

  for (int j=1; j<=n; j++)
  {
    int i1 = bcoff(j).indexmin();
    int i2 = bcoff(j).indexmax();
    //TTRACE(ilb(j),iub(j))
    for (int i=i1; i<=i2; i++)
    {
      bcoff(j,i) = raw_bcoff(j,i)+ z(i,j);
      //bcoff(j,i) += z(i,j);
    }
  }

  for (int j=1; j<=n; j++)
  {
    int i1 = bcoff(j).indexmin();
    int i2 = bcoff(j).indexmax();
    xgam_c.ELEM(j,i1) = 0.0;
    xbet_c.ELEM(j,i1) = 1.0/bcoff.ELEM(j,i1);
    i1++;
    for (int i=i1; i<=i2; i++)
    {
      xgam_c.ELEM(j,i) = ccoff.ELEM(j,i-1) * xbet_c.ELEM(j,i-1);
      xbet_c.ELEM(j,i) = 1.0/(bcoff.ELEM(j,i) - acoff.ELEM(j,i) * xgam_c.ELEM(j,i));
    }
  }
} // end of b_adjust
