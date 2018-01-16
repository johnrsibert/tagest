//$Id: dfb_adj2.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "coff_t.h"

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfb_adjust(coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfz)
{
  dmatrix& acoff = coff.get_a();
  dmatrix& bcoff = coff.get_b();
  dmatrix& ccoff = coff.get_c();
  dmatrix& xbet_c = coff.get_xbet();
  dmatrix& xgam_c = coff.get_xgam();

  dmatrix& dfacoff = get_a();
  dmatrix& dfraw_bcoff = get_raw_b();
  dmatrix& dfbcoff = get_b();
  dmatrix& dfccoff = get_c();
  dmatrix& dfxbet_c = get_xbet();
  dmatrix& dfxgam_c = get_xgam();

	dfb_adjust(
	param,
	dfz,
	acoff,
	bcoff,
	ccoff,
	xbet_c,
	xgam_c,
	dfacoff,
	dfraw_bcoff,
	dfbcoff,
	dfccoff,
	dfxbet_c,
	dfxgam_c);
}
template void coff_t<d3_array,dmatrix,dvector,double>::dfb_adjust(coff_t<d3_array,dmatrix,dvector,double>& coff, par_t<d3_array,dmatrix,dvector,double>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfz);
template<> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfb_adjust(coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& coff, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfz) {}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfb_adjust(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, dmatrix& dfz, dmatrix& acoff, dmatrix& bcoff, dmatrix& ccoff, dmatrix& xbet_c, dmatrix& xgam_c, dmatrix& dfacoff, dmatrix& dfraw_bcoff, dmatrix& dfbcoff, dmatrix& dfccoff, dmatrix& dfxbet_c, dmatrix& dfxgam_c)
{
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
}
template void coff_t<d3_array,dmatrix,dvector,double>::dfb_adjust(par_t<d3_array,dmatrix,dvector,double>& param, dmatrix& dfz, dmatrix& acoff, dmatrix& bcoff, dmatrix& ccoff, dmatrix& xbet_c, dmatrix& xgam_c, dmatrix& dfacoff, dmatrix& dfraw_bcoff, dmatrix& dfbcoff, dmatrix& dfccoff, dmatrix& dfxbet_c, dmatrix& dfxgam_c);
template<> void coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfb_adjust(par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dmatrix& dfz, dmatrix& acoff, dmatrix& bcoff, dmatrix& ccoff, dmatrix& xbet_c, dmatrix& xgam_c, dmatrix& dfacoff, dmatrix& dfraw_bcoff, dmatrix& dfbcoff, dmatrix& dfccoff, dmatrix& dfxbet_c, dmatrix& dfxgam_c) {}
