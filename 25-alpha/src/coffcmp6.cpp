//$Id: coffcmp6.cpp 2754 2011-01-02 20:57:07Z jsibert $
// 08/28/98 Changed conditionals at edge of model domain ragged arrays to
//          correct tag conservation problem. Also changed moving average
//          smoother (uvntrp4.cpp) so that boundary cell velocities != 0 to
//          correct "sticky" edges in low diffusion situations
// 06/08/95 Changed atype-btype-ctype formulations to use the centred-space
//          variable diffusion approximation
//          Imported CHANNEL cell type from coffcmp4.cpp
// 06/07/95 Altered btype boundary formulations for closed and open boundaries
//

// one point difference for dN/dx*dS/dx
#include "coff_t.h"
#include "intersav.h"
//#include "trace.h"
extern ofstream clogf;

#define ELEM elem

template <typename DOUBLE>
DOUBLE smoother(const DOUBLE x, const double scale);

void bad_cell_type(const char* function, int boundary);


double sscale=0.00001;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void Upwind_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::comp(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, const MATRIX& u, const MATRIX& v, const MATRIX& sigma)
{
	MATRIX& acoff = this->get_a();
	MATRIX& bcoff = this->get_raw_b();
	MATRIX& ccoff = this->get_c();
	MATRIX& dcoff = this->get_d();
	MATRIX& ecoff = this->get_e();
	MATRIX& fcoff = this->get_f();
	MATRIX& ygam = this->get_ygam();
	MATRIX& ybet = this->get_ybet();

  //double dx    = param.get_deltax(1);
  double dx    = param.get_deltax_eq();
  double dy    = param.get_deltay();
  double dxdx  = dx * dx;
  double dydy  = dy * dy;
  double twodxdx = 2.0 * dxdx;
  double twodydy = 2.0 * dydy;
  int _m = param.get_m(); //imax;
  int _n = param.get_n(); //jmax;
  ivector jlb1(0,_m+1);
  ivector jub1(0,_m+1);
  for (int i = 1; i <= _m; i++)
  {
   jlb1(i) = jlb(i)-1;
   jub1(i) = jub(i)+1;
  }
  jlb1(0) = 0;
  jlb1(_m+1) = 0;
  jub1(0) = _n+1;
  jub1(_m+1) = _n+1;

  if (param.m_ipar[34]<=0)
  {
    clogf << "param.m_ipar[34] must be greater than 0" << endl;
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

  for (int i=1; i<=_m; i++)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j=j1; j<=jn; j++)
    {
      boundary = param.get_cell_type(i,j);
      //clogf << "*b:" << boundary.b << "*ey:" << boundary.edgey() << endl;
      DOUBLE bsigy = 0.0;
      DOUBLE fsigy = 0.0;
      DOUBLE vm = 0.0;
      DOUBLE vv = v(i,j);
      DOUBLE vp = 0.0;
      if (j>=jlb(i))
      {
        bsigy =  (sigma[i][j] + sigma[i][j-1]) / twodydy;
        vm = v[i][j-1];
      }
      if (j<=jub(i))
      {
        fsigy =  (sigma[i][j] + sigma[i][j+1]) / twodydy;
        vp = v[i][j+1];
      }
      // y direction
      dcoff[i][j] = atype(vm, bsigy, boundary.edgey());
      ecoff[i][j] = btype(vv, bsigy, fsigy, rdt2, boundary.edgey());
      fcoff[i][j] = ctype(vp, fsigy, boundary.edgey());

// this is only true for tags; rtype needs work for growing populations
//    gcoff[i][j] = 0.0; // .initialize()ed in fgcomp

    } // end of j loop
  } // end of i loop


  for (int j=1; j<=_n;j++)
  {
    int i1 = ilb(j);
    int im = iub(j);
    for (int i=i1; i<=im; i++)
    {
      boundary = param.get_cell_type(i,j);

      DOUBLE bsigx = 0.0;
      DOUBLE fsigx = 0.0;
      DOUBLE um = 0.0;
      DOUBLE _uu = u(i,j);
      DOUBLE up = 0.0;
      if ( (j >= jlb1(i-1)) && (j <= jub1(i-1)) )
      {
        bsigx = (sigma[i][j]+sigma[i-1][j]) / twodxdx;
        um = u[i-1][j];
      }
      if ( (j >= jlb1(i+1)) && (j <= jub1(i+1)) )
      {
        fsigx = (sigma[i][j]+sigma[i+1][j]) / twodxdx;
        up = u[i+1][j];         //
      }
 
      // x direction
      acoff[j][i] = atype(um, bsigx, boundary.edgex());
      bcoff[j][i] = btype(_uu, bsigx, fsigx, rdt2, boundary.edgex());
      ccoff[j][i] = ctype(up, fsigx, boundary.edgex());
    } // end of i loop
  } // end of j loop

  for (int i=1; i<=_m; i++)
  {
    int j1 = ecoff(i).indexmin();
    int j2 = ecoff(i).indexmax();

    ygam.ELEM(i,j1) = 0.0;
    ybet.ELEM(i,j1) = 1.0/ecoff.ELEM(i,j1);
    j1++;
    for (int j=j1; j<=j2; j++)
    {
      ygam.ELEM(i,j) = fcoff.ELEM(i,j-1) * ybet.ELEM(i,j-1);
      ybet.ELEM(i,j) = 1.0 /( ecoff.ELEM(i,j) - dcoff.ELEM(i,j) 
                                * ygam.ELEM(i,j));
    }
  }
}
template void Upwind_coff_t<d3_array,dmatrix,dvector,double>::comp(par_t<d3_array,dmatrix,dvector,double>& param, const dmatrix& u, const dmatrix& v, const dmatrix& sigma);
template void Upwind_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::comp(par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, const dvar_matrix& u, const dvar_matrix& v, const dvar_matrix& sigma);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE Upwind_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::atype(const DOUBLE um, const DOUBLE bsig, const int boundary)
{
	DOUBLE acoff = 0;
  switch (boundary)
  {
    case NONE: // n o
    case RIGHT_CLOSED:
      if (um > 0)
        acoff =  -smoother<DOUBLE>(um,sscale) - bsig;
      else
        acoff = -bsig;
      break;
    case RIGHT_OPEN:
      acoff =  -smoother<DOUBLE>(um,sscale);
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
template double Upwind_coff_t<d3_array,dmatrix,dvector,double>::atype(const double um, const double bsig, const int boundary);
template dvariable Upwind_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::atype(const dvariable um, const dvariable bsig, const int boundary);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE Upwind_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::btype(const DOUBLE _uu, const DOUBLE bsig, const DOUBLE fsig, const double dt, const int boundary)
{
	DOUBLE bcoff = 0;
  switch (boundary)
  {
    case LEFT_OPEN:
      bcoff = dt - smoother<DOUBLE>(_uu,sscale);
      break;
    case RIGHT_OPEN:
      bcoff = dt + smoother<DOUBLE>(_uu,sscale);
      break;
    case NONE:
      if (_uu > 0)
        bcoff = dt + (bsig + fsig) + smoother<DOUBLE>(_uu,sscale);
      else
        bcoff = dt + (bsig + fsig) - smoother<DOUBLE>(_uu,sscale);
      break;
    case LEFT_CLOSED:
      if (_uu > 0)
        bcoff = dt + fsig + smoother<DOUBLE>(_uu,sscale);
      else
        bcoff = dt + fsig;
      break;
    case RIGHT_CLOSED:
      if (_uu > 0)
        bcoff = dt + bsig;
      else
        bcoff = dt + bsig - smoother<DOUBLE>(_uu,sscale);
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
template double Upwind_coff_t<d3_array,dmatrix,dvector,double>::btype(const double _uu, const double bsig, const double fsig, const double dt, const int boundary);
template dvariable Upwind_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::btype(const dvariable _uu, const dvariable bsig, const dvariable fsig, const double dt, const int boundary);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
DOUBLE Upwind_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::ctype(const DOUBLE up, const DOUBLE fsig, const int boundary)
{
	DOUBLE ccoff = 0;
  switch (boundary)
  {
    case NONE:
    case LEFT_CLOSED:
      if (up > 0)
        ccoff = -fsig;
      else
        ccoff =  smoother<DOUBLE>(up,sscale) - fsig;
      break;
    case LEFT_OPEN:
      ccoff =  smoother<DOUBLE>(up,sscale);
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
template double Upwind_coff_t<d3_array,dmatrix,dvector,double>::ctype(const double up, const double fsig, const int boundary);
template dvariable Upwind_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::ctype(const dvariable up, const dvariable fsig, const int boundary);

void bad_cell_type(const char* function, int boundary)
{
  cerr << "Unknown cell type (" << setw(4) << hex << (unsigned short)boundary << ") in function "
       << function << endl;
  clogf << "Unknown cell type (" << setw(4) << hex << (unsigned short)boundary << ") in function "
       << function << endl;
  exit(1);
}


