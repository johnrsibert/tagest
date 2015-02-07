#include "precomp.h"
#include <fstream.h>

#undef TRACE
#define TRACE(object) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #object " = " << object << endl;
extern ofstream clogf;

void velpen(par_t_reg& param, double& temp, year_month& date)
{

  int       i,j, k1, k2, k3, k4;
  int m = param.get_m();
  int n = param.get_n();

  //TRACE(param.ipar[33])
  if (param.m_ipar[33] > 0)
  {
    /* allocate storage for differences */

    dmatrix dxu(1,m, 1,n);
    dmatrix dxv(1,m, 1,n);


    int s = param.get_season(date);
    gridpartype_vector& ug = (gridpartype_vector&)param.get_usergrid(s);
    //xpen = param.ipar[31]/1000.;
    //ypen = param.ipar[32]/1000.;
    double xpen = double(param.m_ipar[31])*1.0e-9;
    double ypen = double(param.m_ipar[32])*1.0e-9;
    //TRACE(xpen)
    //TRACE(ypen)

    /* calculate differences in x direction */

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
      }//i loop
    }//j loop

    double d2xu = 0;
    double d2xv = 0;

    for (j=1; j <= n; j++)
    {
      for (i=1; i <= m-2; i++)
      {
	d2xu = dxu[i+1][j] - dxu[i][j];
	temp -= xpen*d2xu*d2xu;

	d2xv = dxv[i+1][j] - dxv[i][j];
	temp -= xpen*d2xv*d2xv;
      }//i loop
    }//j loop
    //TRACE(temp)

  /* rename pointers to keep code understanable? */

    dmatrix& dyu = dxu;
    dmatrix& dyv = dxv;

    double d2yu = 0;
    double d2yv = 0;

  /* calculate differences in y direction */

    for (j=1; j <= n-1; j++)
    {
      for (i=1; i <= m; i++)
      {
	k3 = param.get_gridmap(i, j);
	k4 = param.get_gridmap(i, j+1);
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
      }//i loop
    }//j loop


    for (j=1; j <= n-2; j++)
    {
      for (i=1; i <= m; i++)
      {
	d2yu = dyu[i][j+1] - dyu[i][j];
	temp -= ypen*d2yu*d2yu;

	d2yv = dyv[i][j+1] - dyv[i][j];
	temp -= ypen*d2yv*d2yv;
      }//i loop
    }//j loop
    //TRACE(temp)
/*
    if (fabs(temp) > 1.0)
    {
      clogf << "* * warning: velocity curvature penalty is getting large, " 
            << temp << endl;
    }
*/
  }//if m_ipar[33] > 0
  if (temp > 0) 
  {
    cout << "Error in velpen" << endl;
    clogf << "Error in velpen" << endl;
  }  
}//End of velpen
