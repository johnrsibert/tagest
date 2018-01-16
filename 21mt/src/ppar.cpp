#include "par_t.h"

//iparname.cpp
void set_ipar_names(ipar_t& t);

par_t::par_t(cifstream& prn) : m_ipar(170), Model_area(prn) 
{
  int syr, smon;
  prn >> nmonth >> syr >> smon >> nrelease >> nfleet;
  num_afm = 0;

  allocate();
  initialize();

  get_start_date().set(syr, smon);
  Model_area::read_gridmap(prn);

  set_ipar_names(m_ipar);

  //Default values
  mort = 0.050;
  q = 0.0050;
  report_rates = 1.0;

  minafm = 0.0;
  maxafm = 10.0;
  minsig = 0.0;
  maxsig = 100000.0;
  minmort = 0.0;
  maxmort = 0.5;
  minq = 0.0;
  maxq = 0.1;
  minr_rates = 0.0;
  maxr_rates = 1.0;
  minvel = -10.0;
  maxvel =  10.0;
  fpen = 0.0;
  objective_function_value = 0.0;;
  maximum_gradient = 0.0;
  nvar = 0;


}
