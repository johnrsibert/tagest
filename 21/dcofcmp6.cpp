#include "precomp.h"
#include "intersav.h"
extern ofstream clogf;

void bad_cell_type(const char* function, int boundary);

double smoother( double , double );
double dfsmoother( double , double );


extern double sscale;


void Upwind_coff_t::dfcomp(coff_t& coff, par_t& param, par_t& dfparam,
                           dmatrix& u, dmatrix& v, dmatrix& sigma,
                           year_month& date, dmatrix& dfu, dmatrix& dfv,
                           dmatrix& dfsigma, intersavetype * savefile)
{
  //clogf << "Upwind_coff_t::dfcomp for " << date << endl;
 
  int    i, j;

//  dmatrix& acoff = coff.get_a();
//  dmatrix& bcoff = coff.get_raw_b();
//  dmatrix& ccoff = coff.get_c();
  dmatrix& dcoff = coff.get_d();
  dmatrix& ecoff = coff.get_e();
  dmatrix& fcoff = coff.get_f();
//  dmatrix& gcoff = coff.get_g();
  dmatrix& ybet_c  = coff.get_ybet();
  dmatrix& ygam_c  = coff.get_ygam();

  dmatrix& dfacoff = a;
  dmatrix& dfbcoff = raw_b;
  dmatrix& dfccoff = c;
  dmatrix& dfdcoff = d;
  dmatrix& dfecoff = e;
  dmatrix& dffcoff = f;
//  dmatrix& dfgcoff = g;
  dmatrix& dfybet  = ybet;
  dmatrix& dfygam  = ygam;

  //double dx = param.get_deltax(1);
  double dx = param.get_deltax_eq();
  double dy = param.get_deltay();
  double dxdx  = dx * dx;
  double dydy  = dy * dy;
  double twodxdx = 2.0*dxdx;
  double twodydy = 2.0*dydy;

  int _m     = imax;
  int _n     = jmax;
  
  ivector jlb1(0,_m+1);
  ivector jub1(0,_m+1);
  for (i = 1; i <= _m; i++)
  {
   jlb1(i) = jlb(i)-1;
   jub1(i) = jub(i)+1;
  }
  jlb1(0) = 0;
  jlb1(_m+1) = 0;
  jub1(0) = _n+1;
  jub1(_m+1) = _n+1;
  sscale    = (double)param.m_ipar[34]/100.;

  //int season = param.get_season(date);
  //int month = param.get_month(date);
  //clogf << "dfcoffcalc for " << date << ", season " << season << endl;

  bound boundary;

  double tmp;
  for (i=_m; i>=1; i--)
  {
    int j1 = ecoff(i).indexmin()+1;
    int j2 = ecoff(i).indexmax();
    for ( j=j2; j>=j1; j--)
    {
//->  ybet[i][j] = 1.0 /( ecoff[i][j] - dcoff[i][j] * ygam[i][j]);
      tmp = ecoff[i][j] - dcoff[i][j] * ygam_c[i][j];
      tmp = -dfybet[i][j]/(tmp*tmp);
      dfecoff[i][j] += tmp;
      dfdcoff[i][j] -= tmp*ygam_c[i][j];
      dfygam[i][j] -= tmp*dcoff[i][j];
      dfybet[i][j] = 0.0;

//->  ygam[i][j] = fcoff[i][j-1] * ybet[i][j-1];
      dffcoff[i][j-1] += ybet_c[i][j-1]*dfygam[i][j];
      dfybet[i][j-1] += fcoff[i][j-1]*dfygam[i][j];
      dfygam[i][j] = 0.0;
    }
    j1--;
//->ybet[i][j1] = 1.0/ecoff[i][j1];
    dfecoff[i][j1] -= dfybet[i][j1]/(ecoff[i][j1]*ecoff[i][j1]);
    dfybet[i][j1] = 0.0;

//->ygam[i][j1] = 0.0;
    dfygam[i][j1] = 0.0;
  }

  double dfbsigx = 0.0;
  double dffsigx = 0.0;
  double dfum = 0.0;
  double dfuu = 0.0;
  double dfup = 0.0;

  for ( j = _n; j >= 1; j--)
  {
    int i1 = ilb(j);
    int im = iub(j);
    for ( i = im; i >= i1; i--)
    {
      boundary = param.get_cell_type(i, j);

      // recompute
      double um = 0.0;
      double _uu = u(i,j);
      double up = 0.0;
      if ( (j >= jlb1(i-1)) && (j <= jub1(i-1)) )
      {
        //bsigx = (sigma[i][j]+sigma[i-1][j]) / twodxdx;
        um = u[i-1][j];
      }
      if ( (j >= jlb1(i+1)) && (j <= jub1(i+1)) )
      {
        //fsigx = (sigma[i][j]+sigma[i+1][j]) / twodxdx;
        up = u[i+1][j];         //
      }


      // x direction
      //ctype(ccoff[j][i], up, fsigx, boundary.edgex());
      dfctype(dfccoff[j][i], dfup, dffsigx, up, boundary.edgex());

      //btype(bcoff[j][i], _uu, bsigx, fsigx, rdt2, boundary.edgex());
      dfbtype(dfbcoff[j][i], dfuu, dfbsigx, dffsigx, _uu, boundary.edgex());

      //atype(acoff[j][i], um, bsigx, boundary.edgex());
      dfatype(dfacoff[j][i], dfum, dfbsigx, um, boundary.edgex());

      if ( (j >= jlb1(i+1)) && (j <= jub1(i+1)) )
      {
        //up = u[i+1][j];         //
        dfu[i+1][j] += dfup;
        dfup = 0.0;

        //fsigx = (sigma[i][j]+sigma[i+1][j]) / twodxdx;
        dfsigma[i][j] += dffsigx / twodxdx;
        dfsigma[i+1][j] += dffsigx / twodxdx;
        dffsigx = 0.0;
      }

      if ( (j >= jlb1(i-1)) && (j <= jub1(i-1)) )
      {
        //um = u[i-1][j];
        dfu[i-1][j] += dfum;
        dfum = 0.0;

        //bsigx = (sigma[i][j]+sigma[i-1][j]) / twodxdx;
        dfsigma[i][j] += dfbsigx / twodxdx;
        dfsigma[i-1][j] += dfbsigx / twodxdx;
        dfbsigx = 0.0;
      }

      //double up = 0.0;
      dfup = 0.0;

      //double _uu = u(i,j);
      dfu[i][j] += dfuu;
      dfuu = 0.0;

      //double um = 0.0;
      dfum = 0.0;

      //double fsigx = 0.0;
      dffsigx = 0.0;

      //double bsigx = 0.0;
      dfbsigx = 0.0;
    } // end of i loop
  } // end of j loop

  double dfbsigy = 0.0;
  double dffsigy = 0.0;
  double dfvm = 0.0;
  double dfvv = 0.0;
  double dfvp = 0.0;

  for ( i = _m; i >= 1; i--)
  {
    int j1 = jlb(i);
    int jn = jub(i);
    for ( j = jn; j >= j1; j--)
    {
      boundary = param.get_cell_type(i, j);

      // recompute
      double vm = 0.0;
      double vv = v(i,j);
      double vp = 0.0;
      if (j>=jlb(i))
      {
        //bsigy =  (sigma[i][j] + sigma[i][j-1]) / twodydy;
        vm = v[i][j-1];
      }
      if (j<=jub(i))
      {
        //fsigy =  (sigma[i][j] + sigma[i][j+1]) / twodydy;
        vp = v[i][j+1];
      }

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
        //vp = v[i][j+1];
        dfv[i][j+1] += dfvp;
        dfvp = 0.0;

        //fsigy =  (sigma[i][j] + sigma[i][j+1]) / twodydy;
        dfsigma[i][j] += dffsigy / twodydy;
        dfsigma[i][j+1] += dffsigy / twodydy;
        dffsigy = 0.0;
      }
      if (j>=jlb(i))
      {
        //vm = v[i][j-1];
        //vm = v[i][j-1];
        dfv[i][j-1] += dfvm;
        dfvm = 0.0;

       //bsigy = (sigma[i][j] + sigma[i][j-1]) / twodydy;
        dfsigma[i][j] += dfbsigy / twodydy;
        dfsigma[i][j-1] += dfbsigy / twodydy;
        dfbsigy = 0.0;
      }

      //double vp = 0.0;
      dfvp = 0.0;

      //double vv = v(i,j);
      dfv(i,j)   += dfvv;
      dfvv = 0.0;

      //double vm = 0.0;
      dfvm = 0.0;

      //double fsigy = 0.0;
      dffsigy = 0.0;

      //double bsigy = 0.0;
      dfbsigy = 0.0;
    } // end of j loop
  } // end of i loop



  if (date > param.get_start_date())
  {
    Wrapping_num inc(1, 12, 1);
    year_month t = date - inc;
    //clogf << "|-->";
    param.uvs_comp(u, v, sigma, t);
    coff.comp(param, t, savefile, u, v, sigma);
  }


} /* end of dfcoffcalc */

void Upwind_coff_t::dfctype(double& dfccoff, double& dfup, double& dffsig, double up,
             int boundary)
{
  switch (boundary)
  {
    case NONE:
    case LEFT_CLOSED:
      if (up > 0)
      {
        //ccoff = - fsig;
        dffsig -= dfccoff;
      }
      else
      {
        //ccoff =  smoother(up,sscale) - fsig;
        dfup   += dfsmoother(up,sscale)*dfccoff;
        dffsig -= dfccoff;
      }
      break;
    case LEFT_OPEN:
      //ccoff = smoother(up,sscale);
      dfup   += dfsmoother(up,sscale)*dfccoff;
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

void Upwind_coff_t::dfbtype(double& dfbcoff, double& dfuu, double& dfbsig,
                            double& dffsig, double _uu, int boundary)
{
  switch (boundary)
  {
    case LEFT_OPEN:
      //bcoff = dt - smoother(_uu,sscale);
      dfuu   -= dfsmoother(_uu,sscale) * dfbcoff;
      break;
    case RIGHT_OPEN:
      //bcoff = dt + smoother(_uu,sscale);
      dfuu   += dfsmoother(_uu,sscale) * dfbcoff;
      break;
    case NONE:
      if (_uu > 0)
      {
        //bcoff = dt + (bsig + fsig) + smoother(_uu,sscale);
        dfbsig += dfbcoff;
        dffsig += dfbcoff;
        dfuu   += dfsmoother(_uu,sscale) * dfbcoff;
      }
      else
      {
        //bcoff = dt + (bsig + fsig) - smoother(_uu,sscale);
        dfbsig += dfbcoff;
        dffsig += dfbcoff;
        dfuu   -= dfsmoother(_uu,sscale) * dfbcoff;
      }
      break;
    case LEFT_CLOSED:
      if (_uu > 0)
      {
        //bcoff = dt + fsig + smoother(_uu,sscale);
        dffsig += dfbcoff;
        dfuu   += dfsmoother(_uu,sscale) * dfbcoff;
      }
      else
      {
        //bcoff = dt + fsig;
        dffsig += dfbcoff;
      }
      break;
    case RIGHT_CLOSED:
      if (_uu > 0)
      {
        //bcoff = dt + bsig;
        dfbsig += dfbcoff;
      }
      else
      {
        //bcoff = dt + bsig - smoother(_uu,sscale);
        dfbsig += dfbcoff;
        dfuu   -= dfsmoother(_uu,sscale) * dfbcoff;
      }
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


void Upwind_coff_t::dfatype(double& dfacoff, double& dfum, double& dfbsig, 
                            double um, int boundary)
{
  switch (boundary)
  {
    case NONE:
    case RIGHT_CLOSED:
      if (um > 0)
      {
        //acoff = - smoother(um,sscale) - bsig;
        dfbsig -= dfacoff;
        dfum   -= dfsmoother(um,sscale) * dfacoff;
      }
      else
      {
        //acoff = -bsig;
        dfbsig -= dfacoff;
      }
      break;
    case RIGHT_OPEN:
      //acoff = - smoother(um,sscale);
      dfum   -= dfsmoother(um,sscale) * dfacoff;
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
