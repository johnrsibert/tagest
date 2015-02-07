#include "par_treg.h"

//adstring par_t_reg::par_file_version = "#v20r\0";

void set_ipar_names(ipar_t& t);

void par_t_reg::allocate()
{
  startmonth.allocate(1, nseason);
  usergrid.ptr = new gridpartype_matrix(1, nseason, 0, ngrid);
  stratum_active.allocate(1, nseason, 1, ngrid); 
}

void par_t_reg::initialize()
{
  total_uv_penalty = 0;
  total_sigma_penalty = 0;
  stratum_active.initialize(); 
  for (int s = 1; s <= nseason; s++)
  {
    startmonth(s) = 1+(s-1)*12/nseason;
    for (int k = 1; k <= ngrid; k++)
    {
      usergrid[s][k].u = 0.01;
      usergrid[s][k].v = 0.01;
      usergrid[s][k].sigma = 5000.0;
    }
  }
}

par_t_reg::par_t_reg()
{
  usergrid.ptr = NULL;
}

par_t_reg::par_t_reg(const par_t_reg& t)
: par_t( t ),
  startmonth(t.startmonth), seasons(t.seasons),
  stratum_active(t.stratum_active)
{
  ngrid = t.ngrid;
  nseason = t.nseason;

  usergrid.ptr = new gridpartype_matrix(1, nseason, 0, ngrid);

  nb_par.initialize();
  mort  = 0.0;
  q.initialize();
  report_rates.initialize();
  afm.initialize();
  for (int si=1; si <= nseason; si++)
  {
	  for (int k=0; k <= ngrid; k++)
	  {
	    usergrid[si][k].u = 0.0;
		 usergrid[si][k].v = 0.0;
		 usergrid[si][k].sigma = 0.0;
	  }
  }
}

dvector par_t_reg::get_seasonal_u(const int s)
{
  dvector tmp(1,ngrid);
  for (int i = 1; i <= ngrid; i++)
  {
    tmp(i) = usergrid[s][i].u;
  }
  return (tmp);
}

dvector par_t_reg::get_seasonal_v(const int s)
{
  dvector tmp(1,ngrid);
  for (int i = 1; i <= ngrid; i++)
  {
    tmp(i) = usergrid[s][i].v;
  }
  return (tmp);
}

dvector par_t_reg::get_seasonal_sigma(const int s)
{
  dvector tmp(1,ngrid);
  for (int i = 1; i <= ngrid; i++)
  {
    tmp(i) = usergrid[s][i].sigma;
  }
  return (tmp);
}




par_t_reg::~par_t_reg()
{
  if (usergrid.ptr)
  {
    delete usergrid.ptr;
    usergrid.ptr = NULL;
  }
}

#include "arrays.cpp"

implement(vector, gridpartype_);
implement(matrix, gridpartype_);
