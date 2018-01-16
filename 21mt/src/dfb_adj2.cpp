#include "precomp.h"

extern ofstream clogf;

void coff_t::dfb_adjust(coff_t& coff, par_t& param, par_t& dfparam, 
                        dmatrix& dfz)
{
  dmatrix& acoff = coff.a;
  dmatrix& bcoff = coff.b;
  dmatrix& ccoff = coff.c;
  dmatrix& xbet_c = coff.xbet;
  dmatrix& xgam_c = coff.xgam;

  dmatrix& dfacoff = a;
  dmatrix& dfbcoff = b;
  dmatrix& dfraw_bcoff = raw_b;
  dmatrix& dfccoff = c;
  dmatrix& dfxbet_c = xbet;
  dmatrix& dfxgam_c = xgam;

  int n     = param.get_n();

  for (int j = n; j >= 1; j--)
  {
      int i1 = bcoff(j).indexmin()+1;
      int i2 = bcoff(j).indexmax();
      for (int i = i2; i >= i1; i--)
      {
          double tmp = bcoff[j][i] - acoff[j][i] * xgam_c[j][i];
          tmp = -dfxbet_c[j][i]/(tmp*tmp);
          dfbcoff(j,i) += tmp;
          dfacoff(j,i) -= tmp*xgam_c[j][i];
          dfxgam_c(j,i) -= tmp*acoff[j][i];
          dfxbet_c(j,i) = 0.0;

          dfccoff(j,i-1) += xbet_c[j][i-1]*dfxgam_c[j][i];
          dfxbet_c(j,i-1) += ccoff[j][i-1]*dfxgam_c[j][i];
          dfxgam_c(j,i) = 0.0;
      }
      i1--;
      dfbcoff(j,i1) -= dfxbet_c[j][i1]/(bcoff[j][i1]*bcoff[j][i1]);

      dfxbet_c(j,i1) = 0.0;
      dfxgam_c(j,i1) = 0.0;
  }

  for (int j = n; j >= 1; j--)
  {
      int i1 = bcoff(j).indexmin();
      int i2 = bcoff(j).indexmax();
      for (int i = i2; i >= i1; i--)
      {
          dfraw_bcoff(j,i) += dfbcoff(j,i);
          dfz(i,j) += dfbcoff(j,i);
          dfbcoff(j,i) = 0.0;
      }
  }
} // end of dfb_adjust
