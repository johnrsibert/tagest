#include "precomp2.h"
#include <fvar.hpp>
#include "par_t.h"
extern ofstream clogf;

void total_check_comp(dvector& total_tags, dmatrix& total_check,
		      int month, dmatrix& release, par_t& param,
		      dmatrix& total_mort)
{
  double z = mean(total_mort);
  //double expz =  1. - exp(-z);

  total_tags(month) = sum(release);

  //int nstep = 1;
  //if(param.ipar[6] > 1)
  //  nstep = int(param.ipar[6]);  // time steps per month

  if (month > 0)
  {
  total_check[1][month] = total_check[1][month-1]/(1.0+z); //implicit solution
  total_check[2][month] = total_check[2][month-1]*(1.0-z); //explicit solution
  total_check[3][month] = (total_check[1][month] + total_check[2][month]) * 0.5;
  //total_check[4][month] = total_check[4][month-1]*(1.0-z/2.0)/(1.0+z/2.0);

  total_check[4][month] = total_check[4][month-1]*exp(-z);
  }
  //z = z/nstep;

  //for (i=1; i <=nstep; i++)
  //  total_check[4][month] = total_check[4][month]*(1.0-z/2.0)/(1.0+z/2.0);

  /*
  if (param.m <=6)
  {
    clogf << "\ntotal_check_comp for month " << month << endl;
    for (int j = param.n; j >= 1; j--)
    {
      for (int i = 1; i <= param.m; i++)
      {
	clogf << setw(12) << setprecision(2) << release[i][j];
      }
      clogf << endl;
    }
  }
  */
}//End total_check_comp
