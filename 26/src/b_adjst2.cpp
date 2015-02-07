//$Id: b_adjst2.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "coff_t.h"

#define ELEM elem

adstring effort_file_name(adstring& fleet, year_month& date);

extern ofstream clogf;

void coff_t::b_adjust(const par_t& param, const dvar_matrix& z)
{
  const dvar_matrix& acoff = get_a();
  const dvar_matrix& raw_bcoff = get_raw_b();
  dvar_matrix& bcoff = get_b();
  const dvar_matrix& ccoff = get_c();
  dvar_matrix& xbet_c = get_xbet();
  dvar_matrix& xgam_c = get_xgam();

  int n     = param.get_n();

  //bcoff = raw_bcoff + trans(z);

  for (int j=1; j<=n; j++)
  {
    int i1 = bcoff(j).indexmin();
    int i2 = bcoff(j).indexmax();
    //TTRACE(ilb(j),iub(j))
    for (int i=i1; i<=i2; i++)
    {
      bcoff(j,i) = raw_bcoff(j,i) + z(i,j);
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
}
