//$Id: coffcmp4.cpp 2754 2011-01-02 20:57:07Z jsibert $
// Centred-space differences used for both advection and diffusion terms
// CHANNEL cell type implemented
// fixed "sticky" edges
#include "coff_t.h"
#include "bound.h"
#include "intersav.h"
#define ELEM elem

extern ofstream clogf;

template <typename DOUBLE>
DOUBLE smoother(const DOUBLE x, const double scale);

void bad_cell_type(const char* function, int boundary);

extern double sscale;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::comp(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, const MATRIX& u, const MATRIX& v, const MATRIX& sigma)
{
	MATRIX& acoff = this->get_a();
	MATRIX& bcoff = this->get_raw_b();
	MATRIX& ccoff = this->get_c();
	MATRIX& dcoff = this->get_d();
	MATRIX& ecoff = this->get_e();
	MATRIX& fcoff = this->get_f();
	MATRIX& ybet_c = this->get_ybet();
	MATRIX& ygam_c = this->get_ygam();

  double dx    = param.get_deltax_eq();
  double dy    = param.get_deltay();
  double dxdx  = dx * dx;
  double dydy  = dy * dy;
  double twodxdx = 2.0*dxdx;
  double twodydy = 2.0*dydy;
  int m = param.get_m();
  int n = param.get_n();
  ivector jlb1(0,m+1);
  ivector jub1(0,m+1);
  for (int i = 1; i <= m; i++)
  {
   jlb1(i) = jlb(i)-1;
   jub1(i) = jub(i)+1;
  }
  jlb1(0) = 0;
  jlb1(m+1) = 0;
  jub1(0) = n+1;
  jub1(m+1) = n+1;
  if (param.m_ipar[34]<=0)
  {
    clogf << "param.ipar[34] must be greater than 0" << endl;
    exit(1);
  }
  sscale    = (double)param.m_ipar[34]/100.;

  long nstep;
  if (this->get_k() > 1)
    nstep = this->get_k();  // time steps per month
  else
    nstep = 1;

  double rdt2 = 2*nstep;  // 2/delta_t

  bound boundary;

  for (int i=1; i<=m; i++)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j=j1; j<=jn; j++)
    {
      boundary = param.get_cell_type(i,j);
      DOUBLE bsigy = 0.0;
      DOUBLE fsigy = 0.0;
      DOUBLE vm = 0.0;
      DOUBLE vv = 0.0;
      DOUBLE vp = 0.0;
      if (j>=jlb(i))
      {
        bsigy = (sigma[i][j] + sigma[i][j-1]) / twodydy;
        vm = 0.5*v(i,j-1)/dy;
      }
      if (j<=jub(i))
      {
        fsigy = (sigma[i][j] + sigma[i][j+1]) / twodydy;
        vp = 0.5*v(i,j+1)/dy;
      }
      // y direction
      dcoff[i][j] = atype(vm, bsigy, boundary.edgey());
      ecoff[i][j] = btype(vv, bsigy, fsigy, rdt2, boundary.edgey());
      fcoff[i][j] = ctype(vp, fsigy, boundary.edgey());
    } // end of j loop
  } // end of i loop

  for (int j=1; j<=n;j++)
  {
    int i1 = ilb(j);
    int im = iub(j);
    for (int i=i1; i<=im; i++)
    {
      boundary = param.get_cell_type(i,j);

      DOUBLE bsigx = 0.0;
      DOUBLE fsigx = 0.0;
      DOUBLE um = 0.0;
      DOUBLE uu = 0.0;
      DOUBLE up = 0.0;
      if ( (j >= jlb1(i-1)) && (j <= jub1(i-1)) )
      {
        bsigx = (sigma[i][j]+sigma[i-1][j]) / twodxdx;
        um = 0.5*u(i-1,j)/dx;
      }
      if ( (j >= jlb1(i+1)) && (j <= jub1(i+1)) )
      {
        fsigx = (sigma[i][j]+sigma[i+1][j]) / twodxdx;
        up = 0.5*u(i+1,j)/dx;
      }

      // x direction
      acoff[j][i] = atype(um, bsigx, boundary.edgex());
      bcoff[j][i] = btype(uu, bsigx, fsigx, rdt2, boundary.edgex());
      ccoff[j][i] = ctype(up, fsigx, boundary.edgex());
    } // end of i loop
  } // end of j loop

  for (int i=1; i<=m; i++)
  {
    int j1 = ecoff(i).indexmin();
    int j2 = ecoff(i).indexmax();

    ygam_c(i,j1) = 0.0;
    ybet_c(i,j1) = 1.0/ecoff(i,j1);
    j1++;
    for (int j=j1; j<=j2; j++)
    {
      ygam_c(i,j) = fcoff(i,j-1) * ybet_c(i,j-1);
      ybet_c(i,j) = 1.0 /( ecoff(i,j) - dcoff(i,j) * ygam_c(i,j));
    }
  }
}
template void Centered_coff_t<d3_array,dmatrix,dvector,double>::comp(par_t<d3_array,dmatrix,dvector,double>& param, const dmatrix& u, const dmatrix& v, const dmatrix& sigma);
template void Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::comp(par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, const dvar_matrix& u, const dvar_matrix& v, const dvar_matrix& sigma);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::atype(const DOUBLE um, const DOUBLE bsig, const int boundary)
{
	DOUBLE acoff = 0;
  switch (boundary)
  {
    case NONE:
    case RIGHT_CLOSED:
      acoff = - bsig - smoother<DOUBLE>(um,sscale);
      break;
    case RIGHT_OPEN:
      acoff = - 2.0 * smoother<DOUBLE>(um,sscale);
      break;
    case LEFT_OPEN:
    case LEFT_CLOSED:
    case ISLAND:
    case CHANNEL:
      acoff = 0;
      break;
    default:
      bad_cell_type("atype", boundary);
  }
	return acoff;
}
template double Centered_coff_t<d3_array,dmatrix,dvector,double>::atype(const double um, const double bsig, const int boundary);
template dvariable Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::atype(const dvariable um, const dvariable bsig, const int boundary);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::btype(const DOUBLE uu, const DOUBLE bsig, const DOUBLE fsig, const double dt, const int boundary)
{
	DOUBLE bcoff = 0;
  switch (boundary)
  {
    case LEFT_OPEN:
      bcoff = dt - 2.0 * smoother<DOUBLE>(uu,sscale);
      break;
    case RIGHT_OPEN:
      bcoff = dt + 2.0 * smoother<DOUBLE>(uu,sscale);
      break;
    case NONE:
      bcoff = dt + (bsig + fsig);
      break;
    case LEFT_CLOSED:
      bcoff = dt + fsig + smoother<DOUBLE>(uu,sscale);
      break;
    case RIGHT_CLOSED:
      bcoff = dt + bsig - smoother<DOUBLE>(uu,sscale);
      break;
    case ISLAND:
    case CHANNEL:
      bcoff = dt;
      break;
    default:
      bad_cell_type("btype", boundary);
  }
	return bcoff;
}
template double Centered_coff_t<d3_array,dmatrix,dvector,double>::btype(const double uu, const double bsig, const double fsig, const double dt, const int boundary);
template dvariable Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::btype(const dvariable uu, const dvariable bsig, const dvariable fsig, const double dt, const int boundary);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::ctype(const DOUBLE up, const DOUBLE fsig, const int boundary)
{
	DOUBLE ccoff = 0;
  switch (boundary)
  {
    case NONE:
    case LEFT_CLOSED:
      ccoff =  - fsig + smoother<DOUBLE>(up,sscale);
      break;
    case LEFT_OPEN:
      ccoff = 2.0 * smoother<DOUBLE>(up,sscale);
      break;
    case RIGHT_OPEN:
    case RIGHT_CLOSED:
    case ISLAND:
    case CHANNEL:
      ccoff = 0;
      break;
    default:
      bad_cell_type("ctype", boundary);
  }
	return ccoff;
}
template double Centered_coff_t<d3_array,dmatrix,dvector,double>::ctype(const double up, const double fsig, const int boundary);
template dvariable Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::ctype(const dvariable up, const dvariable fsig, const int boundary);
