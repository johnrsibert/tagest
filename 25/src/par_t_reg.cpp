//$Id: par_t_reg.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"

void set_ipar_names(ipar_t& t);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::allocate()
{
  startmonth.allocate(1, nseason);
  usergridU.allocate(1, nseason, 0, ngrid);
  usergridV.allocate(1, nseason, 0, ngrid);
  usergridSigma.allocate(1, nseason, 0, ngrid);
  stratum_active.allocate(1, nseason, 1, ngrid);
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::allocate();
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::allocate();

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::initialize()
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
template void par_t_reg<d3_array,dmatrix,dvector,double>::initialize();
template void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::initialize();

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t_reg()
{
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t_reg(const par_t_reg<d3_array,dmatrix,dvector,double>& t)
: par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>( t ),
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
template par_t_reg<d3_array,dmatrix,dvector,double>::par_t_reg(const par_t_reg<d3_array,dmatrix,dvector,double>& t);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t_reg(const par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& t)
: par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>( t ),
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
template par_t_reg<d3_array,dmatrix,dvector,double>::par_t_reg(const par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& t);
template par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t_reg(const par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& t);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
VECTOR par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_seasonal_u(const int s)
{
  VECTOR tmp(1,ngrid);
  for (int i = 1; i <= ngrid; i++)
  {
    tmp(i) = usergridU[s][i];
  }
  return (tmp);
}
template dvector par_t_reg<d3_array,dmatrix,dvector,double>::get_seasonal_u(const int s);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
VECTOR par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_seasonal_v(const int s)
{
  VECTOR tmp(1,ngrid);
  for (int i = 1; i <= ngrid; i++)
  {
    tmp(i) = usergridV[s][i];
  }
  return (tmp);
}
template dvector par_t_reg<d3_array,dmatrix,dvector,double>::get_seasonal_v(const int s);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
VECTOR par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::get_seasonal_sigma(const int s)
{
  VECTOR tmp(1,ngrid);
  for (int i = 1; i <= ngrid; i++)
  {
    tmp(i) = usergridSigma[s][i];
  }
  return (tmp);
}
template dvector par_t_reg<d3_array,dmatrix,dvector,double>::get_seasonal_sigma(const int s);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::~par_t_reg()
{
}
template par_t_reg<d3_array,dmatrix,dvector,double>::~par_t_reg();
template par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::~par_t_reg();
