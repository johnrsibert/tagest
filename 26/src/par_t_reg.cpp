//$Id: par_t_reg.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"

void set_ipar_names(ipar_t& t);

void par_t_reg::allocate()
{
  startmonth.allocate(1, nseason);
  usergridU.allocate(1, nseason, 0, ngrid);
  usergridV.allocate(1, nseason, 0, ngrid);
  usergridSigma.allocate(1, nseason, 0, ngrid);
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
      usergridU[s][k] = 0.01;
      usergridV[s][k] = 0.01;
      usergridSigma[s][k] = 5000.0;
    }
  }
}

par_t_reg::par_t_reg()
{
    cerr << "called par_t_reg::par_t_reg()" << endl;
}

par_t_reg::par_t_reg(const par_t_reg& t)
: par_t( t ),
  startmonth(t.startmonth), seasons(t.seasons),
  stratum_active(t.stratum_active)
{
  ngrid = t.ngrid;
  nseason = t.nseason;

  usergridU.allocate(1, nseason, 0, ngrid);
  usergridV.allocate(1, nseason, 0, ngrid);
  usergridSigma.allocate(1, nseason, 0, ngrid);

  nb_par.initialize();
  mort  = 0.0;
  q.initialize();
  report_rates.initialize();
  afm.initialize();
  usergridU.initialize();
  usergridV.initialize();
  usergridSigma.initialize();
}

dvar_vector par_t_reg::get_seasonal_u(const int s)
{
  dvar_vector tmp(1,ngrid);
  for (int i = 1; i <= ngrid; i++)
  {
    tmp(i) = usergridU[s][i];
  }
  return (tmp);
}

dvar_vector par_t_reg::get_seasonal_v(const int s)
{
  dvar_vector tmp(1,ngrid);
  for (int i = 1; i <= ngrid; i++)
  {
    tmp(i) = usergridV[s][i];
  }
  return (tmp);
}

dvar_vector par_t_reg::get_seasonal_sigma(const int s)
{
  dvar_vector tmp(1,ngrid);
  for (int i = 1; i <= ngrid; i++)
  {
    tmp(i) = usergridSigma[s][i];
  }
  return (tmp);
}

par_t_reg::~par_t_reg()
{
}

