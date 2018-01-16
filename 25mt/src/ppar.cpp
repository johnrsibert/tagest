//$Id: ppar.cpp 2920 2012-01-18 02:51:43Z eunjung $
#include "par_t.h"
#include "trace.h"

//template <>
//par_t<d3_array,dmatrix,dvector,double>::par_t(cifstream& prn) : Model_area(prn), m_ipar(100)
/** Creates instance of par_t base class from skeleton .par file
Sets the number of switches to 100.
Assigns defaults values to some member variables just to be safe.
\param prn reference to an open .prn file
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t(cifstream& prn) : Model_area(prn), m_ipar(100)
{
  int syr, smon;
  prn >> nmonth >> syr >> smon >> nrelease >> nfleet;
  TRACE(nfleet)
  num_afm = 0;

  allocate();
  initialize();

  get_start_date().set(syr, smon);
  Model_area::read_gridmap(prn);

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

  minsus_spd = 0.0;
  maxsus_spd = 1.0;
  //minMin_D = 0.0;
  //maxMin_D = 100.0;
  minslope = -10.0;
  maxslope =  10.0;
  //mininflection = 0.0;
  //maxinflection = 100.0;

  fpen = 0.0;
  total_uv_penalty = 0.0;
  total_sigma_penalty = 0.0;
  total_negative_tag_penalty = 0.0;
  total_total_tag_penalty = 0.0;
  total_bounds_penalty = 0.0;
  total_likelihood_contribution= 0.0;
  objective_function_value = 0.0;;
  maximum_gradient = 0.0;
  nvar = 0;
}
template par_t<d3_array,dmatrix,dvector,double>::par_t(cifstream& prn);
template par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t(cifstream& prn);


