#include "precomp.h"
#undef TRACE
#define TRACE(object) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #object " = " << object << endl;

extern ofstream clogf;

void dfvelpen(par_t_reg& dfparam, par_t_reg& param, year_month& date)
{

  int       i,j, k1, k2, k3, k4;
  double  dftemp = 1;
  int m = param.get_m();
  int n = param.get_n();

  //TRACE(param.ipar[33])
  if (param.m_ipar[33] > 0)
  {

  /* allocate storage for differences */

    dmatrix dyu(1,m, 1,n);
    dmatrix dyv(1,m, 1,n);
    //dmatrix d2yu(1,m, 1,n);
    //dmatrix d2yv(1,m, 1,n);

    dmatrix dfdyu(1,m, 1,n);
    dmatrix dfdyv(1,m, 1,n);


    int s = param.get_season(date);
    gridpartype_vector& dfug = (gridpartype_vector&) dfparam.get_usergrid(s);
    gridpartype_vector&   ug = (gridpartype_vector&)   param.get_usergrid(s);

    double xpen = double(param.m_ipar[31])*1.0e-9;
    double ypen = double(param.m_ipar[32])*1.0e-9;


 /* re- calculate differences in y direction */

    for (j=1; j <= n-1; j++)
    {
      for (i=1; i <= m; i++)
      {
	k3 = param.get_gridmap(i, j);
	k4 = param.get_gridmap(i, j+1);  /* = k2 */
	if ( (k3>0) && (k4>0) )
	{
	  dyu(i,j) = 30.0*(ug[k4].u -  ug[k3].u);
	  dyv(i,j) = 30.0*(ug[k4].v -  ug[k3].v);
	}
	else
	{
	  dyu(i,j) = 0.0;
	  dyv(i,j) = 0.0;
	}
      }
    }

    double d2yu = 0.0;
    double d2yv = 0.0;

    dfdyu.initialize();
    dfdyv.initialize();
    double dfd2yu = 0.0;
    double dfd2yv = 0.0;

    for (j = n-2; j >= 1; j--)
    {
	for (i = m; i>=1; i--)
	{
	   d2yv = dyv[i][j+1] - dyv[i][j];

	   // temp -= ypen*d2yv*d2yv;
	   dfd2yv -= 2.0*ypen*d2yv*dftemp;
	   //d2yv = dyv[i][j+1] - dyv[i][j];
	   dfdyv[i][j+1] += dfd2yv;
	   dfdyv[i][j] -= dfd2yv;
	   dfd2yv = 0.0;

	   d2yu = dyu[i][j+1] - dyu[i][j];

	   //temp -= ypen*d2yu*d2yu;
	   dfd2yu -= 2.0*ypen*d2yu*dftemp;
	   //d2yu = dyu[i][j+1] - dyu[i][j];
	   dfdyu[i][j+1] += dfd2yu;
	   dfdyu[i][j] -= dfd2yu;
	   dfd2yu = 0.0;
      }
    }

    /* calculate differences in y direction */

    for (j = n-1; j >=1; j--)
    {
	for (i = m; i >= 1; i--)
	{
	    k3 = param.get_gridmap(i, j);
	    k4 = param.get_gridmap(i, j+1);    /* = k2 */

	    if ( (k3>0) && (k4>0) )
	    {
	      /* dyv[i][j] = 30.0*(ug[k4].v - ug[k3].v); */
	      dfug[k4].v += 30.0*dfdyv[i][j];
	      dfug[k3].v -= 30.0*dfdyv[i][j];

	      /* dyu[i][j] = 30.0*(ug[k4].u - ug[k3].u); */
	      dfug[k4].u += 30.0*dfdyu[i][j];
	      dfug[k3].u -= 30.0*dfdyu[i][j];
	    }
	    else
	    {
	      //dyv(i,j) = 0.0;
	      dfdyv(i,j) = 0.0;
	      //dyu(i,j) = 0.0;
	      dfdyu(i,j) = 0.0;
	    }
	}
    }



    dmatrix& dxu = dyu;
    dmatrix& dxv = dyv;
    //dmatrix& d2xu = d2yu;
    //dmatrix& d2xv = d2yv;

   /* re-calculate differences in x direction */

    for (j=1; j <= n; j++)
    {
      for (i=1; i <= m-1; i++)
      {
	k1 = param.get_gridmap(i, j);
        k2 = param.get_gridmap(i+1, j);
	if ( (k1>0) && (k2>0) )
	{
	  dxu(i,j) = 30.0*(ug[k2].u -  ug[k1].u);
	  dxv(i,j) = 30.0*(ug[k2].v -  ug[k1].v);
	}
	else
	{
	  dxu(i,j) = 0.0;
	  dxv(i,j) = 0.0;
	}
      }
    }

    dmatrix& dfdxu = dfdyu;
    dmatrix& dfdxv = dfdyv;
    dfdxu.initialize();
    dfdxv.initialize();
    //ypen = param.ipar[32]/1000.;
    //xpen = param.ipar[31]/1000.;

    double d2xu = 0.0;
    double d2xv = 0.0;
    double dfd2xu = 0.0;
    double dfd2xv = 0.0;

    for (j = n; j >= 1; j--)
    {
	for (i = m-2; i>=1; i--)
	{
	  //recompute
	  d2xu = dxu[i+1][j] - dxu[i][j];

	  //temp -= xpen*d2xu*d2xu;
	  dfd2xu -= 2.0*xpen*d2xu*dftemp;
	  //d2xu = dxu[i+1][j] - dxu[i][j];
	  dfdxu[i+1][j] += dfd2xu;
	  dfdxu[i][j] -= dfd2xu;
	  dfd2xu = 0.0;

	  // recompute
	  d2xv = dxv[i+1][j] - dxv[i][j];

	  //temp -= xpen*d2xv*d2xv;
	  dfd2xv -= 2.0*xpen*d2xv*dftemp;
	  //d2xv = dxv[i+1][j] - dxv[i][j];
	  dfdxv[i+1][j] += dfd2xv;
	  dfdxv[i][j] -= dfd2xv;
	  dfd2xv = 0.0;
	}
    }

    /* calculate differences in x direction */

    for (j = n; j >=1; j--)
    {
	for (i = m-1; i >= 1; i--)
	{
	    k1 = param.get_gridmap(i, j);
	    k2 = param.get_gridmap(i+1, j);
	    if ( (k1>0) && (k2>0) )
	    {

	      /* dxv[i][j] = 30.0*(ug[k2].v - ug[k1].v); */
	      dfug[k2].v += 30.0*dfdxv[i][j];
	      dfug[k1].v -= 30.0*dfdxv[i][j];
	      dfdxv[i][j] = 0.0;

	      /* dxu[i][j] = 30.0*(ug[k2].u - ug[k1].u); */
	      dfug[k2].u += 30.0*dfdxu[i][j];
	      dfug[k1].u -= 30.0*dfdxu[i][j];
	      dfdxu[i][j] = 0.0;
	    }
	    else
	    {
	      //dxv(i,j) = 0.0;
	      dfdxv(i,j) = 0.0;
	      //dxu(i,j) = 0.0;
	      dfdxu(i,j) = 0.0;
	    }
	}
    }
  }
} /* end of dfvelpen */
