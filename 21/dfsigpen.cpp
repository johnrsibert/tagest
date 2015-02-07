#include "precomp.h"

extern ofstream clogf;

void dfsigpen(par_t_reg& dfparam, par_t_reg& param, year_month& date)
{
  int       i,j, k1, k2, k3, k4;

  //TRACE(param.ipar[41])
  if (param.m_ipar[41] > 0)
  {
    double  dftemp = 1;
    int m = param.get_m();
    int n = param.get_n();

    dmatrix dysigma(1,m, 1,n);
    dmatrix dfdysigma(1,m, 1,n);


    int s = param.get_season(date);
    //clogf << "dfsigpen for " << date << "; season = " << s << endl;
    gridpartype_vector& dfug = (gridpartype_vector&)dfparam.get_usergrid(s);
    gridpartype_vector&   ug =   (gridpartype_vector&)param.get_usergrid(s);

    int ngrid = param.get_ngrid();
    dvector sigma(1,ngrid);
    dvector dfsigma(1,ngrid);
    dfsigma.initialize();
    // recompute
    for (int k = 1; k <= ngrid; k++)
    {
      sigma[k] = sqrt(ug[k].sigma/M_PI);
      //sigma[k] = sqrt(param.get_usergrid(s, k).sigma/M_PI);
    }

    double sigpen = double(param.m_ipar[42])*1.0e-9;
//    double sigpen = param.m_ipar[42]/1000.;

 /* re- calculate differences in y direction */

    for (j=1; j <= n-1; j++)
    {
      for (i=1; i <= m; i++)
      {
	k3 = param.get_gridmap(i, j);
	k4 = param.get_gridmap(i, j+1);  /* = k2 */
	if ( (k3>0) && (k4>0) )
	{
	  dysigma(i,j) = sigma[k4] - sigma[k3];
	}
	else
	{
	  dysigma(i,j) = 0.0;
	}
      }
    }
    double d2ysigma = 0.0;

    dfdysigma.initialize();
    double dfd2ysigma = 0.0;

    for (j = n-2; j >= 1; j--)
    {
	for (i = m; i>=1; i--)
	{
	   d2ysigma = dysigma[i][j+1] - dysigma[i][j];

	   //temp -= sigpen*d2ysigma*d2ysigma;
	   dfd2ysigma -= 2.0*sigpen*d2ysigma*dftemp;
	   //d2ysigma = dysigma[i][j+1] - dysigma[i][j];
	   dfdysigma[i][j+1] += dfd2ysigma;
	   dfdysigma[i][j] -= dfd2ysigma;
	   dfd2ysigma = 0.0;
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
	      /* dysigma[i][j] = sigma[k4] - sigma[k3]; */
	      dfsigma[k4] += dfdysigma[i][j];
	      dfsigma[k3] -= dfdysigma[i][j];
	      dfdysigma(i,j) = 0.0;
	    }
	    else
	    {
	      //dysigma(i,j) = 0.0;
	      dfdysigma(i,j) = 0.0;
	    }
	}
    }

    dmatrix& dxsigma = dysigma;
   /* re-calculate differences in x direction */
    for (j=1; j <= n; j++)
    {
      for (i=1; i <= m-1; i++)
      {
	k1 = param.get_gridmap(i, j);
	k2 = param.get_gridmap(i+1, j);
	if ( (k1>0) && (k2>0) )
	{
	  dxsigma(i,j) = sigma[k2] -  sigma[k1];
	}
	else
	{
	  dxsigma(i,j) = 0.0;
	}
      }
    }

    dmatrix& dfdxsigma = dfdysigma;
    dfdxsigma.initialize();

    double d2xsigma = 0.0;
    double dfd2xsigma = 0.0;

    for (j = n; j >= 1; j--)
    {
	for (i = m-2; i>=1; i--)
	{
	  //recompute
	  d2xsigma = dxsigma[i+1][j] - dxsigma[i][j];

	  //temp -= sigpen*d2xsigma*d2xsigma;
	  dfd2xsigma -= 2.0*sigpen*d2xsigma*dftemp;
	  //d2xsigma = dxsigma[i+1][j] - dxsigma[i][j];
	  dfdxsigma[i+1][j] += dfd2xsigma;
	  dfdxsigma[i][j] -= dfd2xsigma;
	  dfd2xsigma = 0.0;
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
	      dfsigma[k2] += dfdxsigma[i][j];
	      dfsigma[k1] -= dfdxsigma[i][j];
	      dfdxsigma[i][j] = 0.0;
	    }
	    else
	    {
	      //dxsigma(i,j) = 0.0;
	      dfdxsigma(i,j) = 0.0;
	    }
	}
    }
    // convert sigma to a radius
    double root_pi_1 = 1.0/sqrt(M_PI);
    for (int k = ngrid; k >= 1; k--)
    {
      //sigma[k] = sqrt(ug[k].sigma/M_PI);
      dfug[k].sigma += dfsigma[k]*root_pi_1*0.5/sqrt(ug[k].sigma);
      dfsigma[k] = 0.0;
    }
  }
} /* end of dfsigpen */
