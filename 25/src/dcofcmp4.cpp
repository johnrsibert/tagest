//$Id: dcofcmp4.cpp 2754 2011-01-02 20:57:07Z jsibert $
// Centred-space differences used for both advection and diffusion terms
// CHANNEL cell type implemented

#include "coff_t.h"
#include "bound.h"
#include "intersav.h"

double dfsmoother( double , double );
void bad_cell_type(const char* function, int boundary);

extern double sscale;
extern ofstream clogf;


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfcomp(coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& date, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, intersavetype * savefile)
{
  dmatrix& dcoff = coff.get_d();
  dmatrix& ecoff = coff.get_e();
  dmatrix& fcoff = coff.get_f();
  dmatrix& ybet_c = coff.get_ybet();
  dmatrix& ygam_c = coff.get_ygam();

  dmatrix& dfacoff = get_a();
  dmatrix& dfbcoff = get_raw_b();
  dmatrix& dfccoff = get_c();
  dmatrix& dfdcoff = get_d();
  dmatrix& dfecoff = get_e();
  dmatrix& dffcoff = get_f();
  dmatrix& dfybet = get_ybet();
  dmatrix& dfygam = get_ygam();
	dfcomp(coff, param,
                           u, v, sigma,
                           date, dfu, dfv,
                           dfsigma, savefile,
dcoff,
ecoff,
fcoff,
ybet_c,
ygam_c,
dfacoff,
dfbcoff,
dfccoff,
dfdcoff,
dfecoff,
dffcoff,
dfybet,
dfygam);
}
template void Centered_coff_t<d3_array,dmatrix,dvector,double>::dfcomp(coff_t<d3_array,dmatrix,dvector,double>& coff, par_t<d3_array,dmatrix,dvector,double>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& date, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, intersavetype * savefile);
template<> void Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfcomp(coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& coff, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& date, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, intersavetype * savefile) {}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfcomp(coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param,
                           dmatrix& u, dmatrix& v, dmatrix& sigma,
                           year_month& date, dmatrix& dfu, dmatrix& dfv,
                           dmatrix& dfsigma, intersavetype * savefile,
dmatrix& dcoff,
dmatrix& ecoff,
dmatrix& fcoff,
dmatrix& ybet_c,
dmatrix& ygam_c,
dmatrix& dfacoff,
dmatrix& dfbcoff,
dmatrix& dfccoff,
dmatrix& dfdcoff,
dmatrix& dfecoff,
dmatrix& dffcoff,
dmatrix& dfybet,
dmatrix& dfygam)
{
  //clogf << "Upwind_coff_t::dfcomp for " << date << endl;
  //int    i, j, k;


  double dx = param.get_deltax_eq();
  double dy = param.get_deltay();
  double dxdx  = dx * dx;
  double dydy  = dy * dy;
  double twodxdx = 2.0*dxdx;
  double twodydy = 2.0*dydy;

  int _m     = param.get_m();
  int _n     = param.get_n();
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

  sscale    = (double)param.m_ipar[34]/100.;

  bound boundary;

  double tmp;
  for (int i=_m; i>=1; i--)
  {
    int j1 = ecoff(i).indexmin()+1;
    int j2 = ecoff(i).indexmax();
    for (int j=j2; j>=j1; j--)
    {
//      ybet[i][j] = 1.0 /( ecoff[i][j] - dcoff[i][j] * ygam[i][j]);
      tmp = ecoff[i][j] - dcoff[i][j] * ygam_c[i][j];
      tmp = -dfybet[i][j]/(tmp*tmp);
      dfecoff[i][j] += tmp;
      dfdcoff[i][j] -= tmp*ygam_c[i][j];
      dfygam[i][j] -= tmp*dcoff[i][j];
      dfybet[i][j] = 0.0;

//      ygam[i][j] = fcoff[i][j-1] * ybet[i][j-1];
      dffcoff[i][j-1] += ybet_c[i][j-1]*dfygam[i][j];
      dfybet[i][j-1] += fcoff[i][j-1]*dfygam[i][j];
      dfygam[i][j] = 0.0;
    }
    j1--;
//    ybet[i][j1] = 1.0/ecoff[i][j1];
    dfecoff[i][j1] -= dfybet[i][j1]/(ecoff[i][j1]*ecoff[i][j1]);
    dfybet[i][j1] = 0.0;

//    ygam[i][j1] = 0.0;
    dfygam[i][j1] = 0.0;
  }

  double dfbsigx = 0.0;
  double dffsigx = 0.0;
  double dfum = 0.0;
  double dfuu = 0.0;
  double dfup = 0.0;

  for (int j=_n; j>=1; j--)
  {
    int i1 = ilb(j);
    int im = iub(j);
    for (int i=im; i>=i1; i--)
    {
      // recompute
      double um = 0.0;
      double uu = 0.0;
      double up = 0.0;
      if ( (j >= jlb1(i-1)) && (j <= jub1(i-1)) )
      {
        um = 0.5*u(i-1,j)/dx;
      }
      if ( (j >= jlb1(i+1)) && (j <= jub1(i+1)) )
      {
        up = 0.5*u(i+1,j)/dx;
      }

      boundary = param.get_cell_type(i,j);

      // x direction
      //ctype(ccoff[j][i], up, fsigx, boundary.edgex());
      dfctype(dfccoff[j][i], dfup, dffsigx, up, boundary.edgex());

      //btype(bcoff[j][i], uu, bsigx, fsigx, rdt2, boundary.edgex());
      dfbtype(dfbcoff[j][i], dfuu, dfbsigx, dffsigx, uu, boundary.edgex());

      //atype(acoff[j][i], um, bsigx, boundary.edgex());
      dfatype(dfacoff[j][i], dfum, dfbsigx, um, boundary.edgex());

      if ( (j >= jlb1(i+1)) && (j <= jub1(i+1)) )
      {
        //up = 0.5*u(i+1,j);
        dfu(i+1,j) += 0.5*dfup/dx;

        //fsigx = (sigma[i][j]+sigma[i+1][j]) / twodxdx;
        dfsigma[i][j] += dffsigx / twodxdx;
        dfsigma[i+1][j] += dffsigx / twodxdx;
      }
      if ( (j >= jlb1(i-1)) && (j <= jub1(i-1)) )
      {
        //um = 0.5*u(i-1,j);
        dfu(i-1,j) += 0.5*dfum/dx;

        //bsigx = (sigma[i][j]+sigma[i-1][j]) / twodxdx;
        dfsigma[i][j] += dfbsigx / twodxdx;
        dfsigma[i-1][j] += dfbsigx / twodxdx;
      }

      //double bsigx = 0.0;
        dfbsigx = 0.0;
      //double fsigx = 0.0;
        dffsigx = 0.0;
      //double um = 0.0;
        dfum = 0.0;
      //double uu = 0.0;
        dfuu = 0.0;
      //double up = 0.0;
        dfup = 0.0;

    } // end of i loop
  } // end of j loop

  double dfbsigy = 0.0;
  double dffsigy = 0.0;
  double dfvm = 0.0;
  double dfvv = 0.0;
  double dfvp = 0.0;

  for (int i=_m; i>=1; i--)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for (int j=jn; j>=j1; j--)
    {
      // recompute
      double vm = 0.0;
      double vv = 0.0;
      double vp = 0.0;
      if (j>=jlb(i))
      {
        vm = 0.5*v(i,j-1)/dy;
      }
      if (j<=jub(i))
      {
        vp = 0.5*v(i,j+1)/dy;
      }

      boundary = param.get_cell_type(i,j);

// this is only true for tags; rtype needs work for growing populations
//      gcoff[i][j] = 0.0;

      // y direction
      //ctype(fcoff[i][j], vp, fsigy, boundary.edgey());
      dfctype(dffcoff[i][j], dfvp, dffsigy, vp, boundary.edgey());

      //btype(ecoff[i][j], vv, bsigy, fsigy, rdt2, boundary.edgey());
      dfbtype(dfecoff[i][j], dfvv, dfbsigy, dffsigy, vv, boundary.edgey());

      //atype(dcoff[i][j], vm, bsigy, boundary.edgey());
      dfatype(dfdcoff[i][j], dfvm, dfbsigy, vm, boundary.edgey());

      if (j<=jub(i))
      {
        //vp = 0.5*v(i,j+1);
        dfv(i,j+1) += 0.5*dfvp/dy;

        //fsigy = (sigma[i][j] + sigma[i][j+1]) / twodydy;
        dfsigma[i][j] += dffsigy / twodydy;
        dfsigma[i][j+1] += dffsigy / twodydy;
      }

      if (j>=jlb(i))
      {
        //vm = 0.5*v(i,j-1);
        dfv(i,j-1) += 0.5*dfvm/dy;

        //bsigy = (sigma[i][j] + sigma[i][j-1]) / twodydy;
        dfsigma[i][j] += dfbsigy / twodydy;
        dfsigma[i][j-1] += dfbsigy / twodydy;
      }

      //double bsigy = 0.0;
      dfbsigy = 0.0;
      //double fsigy = 0.0;
      dffsigy = 0.0;
      //double vm = 0.0;
      dfvm = 0.0;
      //double vv = 0.0;
      dfvv = 0.0;
      //double vp = 0.0;
      dfvp = 0.0;

    } // end of j loop
  } // end of i loop

  if (date > param.get_start_date())
  {
    Wrapping_num inc(1, 12, 1);
    year_month t = date - inc;

    //clogf << "|-->";
    param.uvs_comp(u, v, sigma, t);

    coff.clean();
    coff.comp(param, u, v, sigma);
  }

} /* end of dfcoffcalc */
template void Centered_coff_t<d3_array,dmatrix,dvector,double>::dfcomp(coff_t<d3_array,dmatrix,dvector,double>& coff, par_t<d3_array,dmatrix,dvector,double>& param, dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& date, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, intersavetype * savefile, dmatrix& dcoff, dmatrix& ecoff, dmatrix& fcoff, dmatrix& ybet_c, dmatrix& ygam_c, dmatrix& dfacoff, dmatrix& dfbcoff, dmatrix& dfccoff, dmatrix& dfdcoff, dmatrix& dfecoff, dmatrix& dffcoff, dmatrix& dfybet, dmatrix& dfygam);
template void Centered_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::dfcomp(coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& coff, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dmatrix& u, dmatrix& v, dmatrix& sigma, year_month& date, dmatrix& dfu, dmatrix& dfv, dmatrix& dfsigma, intersavetype * savefile, dmatrix& dcoff, dmatrix& ecoff, dmatrix& fcoff, dmatrix& ybet_c, dmatrix& ygam_c, dmatrix& dfacoff, dmatrix& dfbcoff, dmatrix& dfccoff, dmatrix& dfdcoff, dmatrix& dfecoff, dmatrix& dffcoff, dmatrix& dfybet, dmatrix& dfygam);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfctype(double& dfccoff, double& dfup, double& dffsig, double up, int boundary)
{
  switch (boundary)
  {
    case NONE:
    case LEFT_CLOSED:
      //ccoff = - fsig + smoother(up,sscale);
      dffsig -= dfccoff;
      dfup   += dfsmoother(up,sscale)*dfccoff;
      break;
    case LEFT_OPEN:
      //ccoff = 2.0 * smoother(up,sscale);
      dfup   += 2.0 * dfsmoother(up,sscale)*dfccoff;
      break;
    case RIGHT_OPEN:
    case RIGHT_CLOSED:
    case ISLAND:
    case CHANNEL:
      //ccoff = 0;
      break;
    default:
      bad_cell_type("dfctype", boundary);
  }
  dfccoff = 0.0;
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfbtype(double& dfbcoff, double& dfuu, double& dfbsig, double& dffsig, double uu, int boundary)
{
  switch (boundary)
  {
    case LEFT_OPEN:
      //bcoff = dt - 2.0 * smoother(uu,sscale);
      dfuu   -= 2.0 * dfsmoother(uu,sscale)*dfbcoff;
      break;
    case RIGHT_OPEN:
      //bcoff = dt + 2.0 * smoother(uu,sscale);
      dfuu   += 2.0 * dfsmoother(uu,sscale)*dfbcoff;
      break;
    case NONE:
      //bcoff = dt + (bsig + fsig);
      dfbsig += dfbcoff;
      dffsig += dfbcoff;
      break;
    case LEFT_CLOSED:
      //bcoff = dt + fsig + smoother(uu,sscale);
      dffsig += dfbcoff;
      dfuu   += dfsmoother(uu,sscale)*dfbcoff;
      break;
    case RIGHT_CLOSED:
      //bcoff = dt + bsig - smoother(uu,sscale);
      dfbsig += dfbcoff;
      dfuu   -= dfsmoother(uu,sscale)*dfbcoff;
      break;
    case ISLAND:
    case CHANNEL:
      //bcoff = dt;
      break;
    default:
      bad_cell_type("dfbtype", boundary);
  }
  dfbcoff = 0.0;
}


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void Centered_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfatype(double& dfacoff, double& dfum, double& dfbsig, double um, int boundary)
{
  switch (boundary)
  {
    case NONE:
    case RIGHT_CLOSED:
      //acoff = - bsig - smoother(um,sscale);
      dfbsig -= dfacoff;
      dfum   -= dfsmoother(um,sscale)*dfacoff;
      break;
    case RIGHT_OPEN:
      //acoff = - 2.0 * smoother(um,sscale);
      dfum   -= 2.0 * dfsmoother(um,sscale)*dfacoff;
      break;
    case LEFT_OPEN:
    case LEFT_CLOSED:
    case ISLAND:
    case CHANNEL:
      //acoff = 0;
      break;
    default:
      bad_cell_type("dfatype", boundary);
  }
  dfacoff = 0.0;
}
