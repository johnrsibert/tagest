//$Id: par_t.cpp 2949 2012-02-13 02:04:24Z jsibert $
#pragma implementation "par_t.h"
#include "par_t.h"
#include "trace.h"
#include "ipar.h"

extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::instance = 0;

template par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
template par_t<d3_array,dmatrix,dvector,double>* par_t<d3_array,dmatrix,dvector,double>::instance;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
static void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::deleteInstance() 
{
  if (instance != 0) {
    delete instance;
    instance = 0;
  }
}

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::allocate()
{
  HERE
  if (nrelease > 0)
    tr.ptr = new tagreleasetype_vector(1,nrelease);
  else
    cout << "Warning: par_t::nrelease = " << nrelease << endl;

  HERE
  q.allocate(1,nfleet);
  report_rates.allocate(1,nfleet);
  fleet_names.allocate(1,nfleet);
  DomesticFleetKludgeFlag.allocate(1,nfleet);
  nb_par.allocate(1,nfleet);
  nb_par.initialize();
  minafm = 0.0;
  maxafm = 10.0;
  TRACE(num_afm)
  if (num_afm)
  {
     cerr << "fishing mortality anomailes (ipar[24]) no longer supported." << endl;
     ad_exit(1);
    //minafm = 0.0;
    //maxafm = 10.0;
    afm_yr.allocate(1,num_afm);
    afm_mo.allocate(1,num_afm);
    afm_fl.allocate(1,num_afm);
    afm_i.allocate(1,num_afm);
    afm_j.allocate(1,num_afm);
    afm.allocate(1,num_afm);
  }
  HERE
}
template void par_t<d3_array,dmatrix,dvector,double>::allocate();
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::allocate();

/** Initialize base class variables.
Duplicates some functions of par_t(cifstream&). */
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::initialize()
{
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
template void par_t<d3_array,dmatrix,dvector,double>::initialize();
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::initialize();

/** Null constructor. Sets the tr.ptr to NULL. */
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t()
{
  //cout << "Top of par_t null constructor" << endl;
  tr.ptr = NULL;
  //cout << "Bottom of par_t null constructor" << endl;
}
template par_t<d3_array,dmatrix,dvector,double>::par_t();
template par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t();

/** copy constructor #1.
\callergraph
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t(const par_t<d3_array,dmatrix,dvector,double>& t)
: Model_area( t ),
  m_ipar(t.m_ipar), //cohort_index(t.cohort_index),  
  fleet_names(t.fleet_names),start_date(t.start_date),//tr(t.tr),
  afm_yr(t.afm_yr),afm_mo(t.afm_mo),afm_fl(t.afm_fl),afm_i(t.afm_i),
  afm_j(t.afm_j)
{
  cout << "Top of par_t copy constructor #1" << endl;

  nmonth = t.nmonth;
  nrelease =  t.nrelease;
  nfleet= t.nfleet;

  //tr.ptr = t.tr.ptr;
  tr.ptr = new tagreleasetype_vector(*(t.tr.ptr));

  mort =  t.mort;
  special_mort = t.special_mort;
  q.allocate(1,nfleet);
  q = t.q;
  report_rates.allocate(1,nfleet);
  report_rates = t.report_rates;
  nb_par.allocate(1,nfleet);
  nb_par = t.nb_par;

  num_afm=t.num_afm;
  if (num_afm)
  {
    minafm = t.minafm;
    maxafm = t.maxafm;
    afm.allocate(1,num_afm);
    afm = t.afm;
  }

  minsig = t.minsig;
  maxsig = t.maxsig;
  minmort = t.minmort;
  maxmort = t.maxmort;
  minq = t.minq;
  maxq = t.maxq;
  minr_rates = t. minr_rates;
  maxr_rates = t.maxr_rates;
  minvel = t.minvel;
  maxvel = t.maxvel;

  minsus_spd = t.minsus_spd;
  maxsus_spd = t.maxsus_spd;
  //minMin_D = t.minMin_D;
  //maxMin_D = t.maxMin_D;
  minslope = t.minslope;
  maxslope = t.maxslope;
  //mininflection = t.mininflection;
  //maxinflection = t.maxinflection;

  fpen = t.fpen;
  total_uv_penalty = t.total_uv_penalty;
  total_sigma_penalty = t.total_sigma_penalty;
  total_negative_tag_penalty = t.total_negative_tag_penalty;
  total_total_tag_penalty = t.total_total_tag_penalty;
  total_bounds_penalty = t.total_bounds_penalty;
  total_likelihood_contribution= t.total_likelihood_contribution;
  objective_function_value = t.objective_function_value;
  maximum_gradient = t.maximum_gradient;
  nvar = t. nvar;
  //
  //for (int i = 1; i <= nrelease; i++)
  //{
  //  clogf << t.tr[i] << endl;
  //  clogf << tr[i] << endl << endl;
  //}
  //
  //cout << "Bottom of par_t copy constructor #1" << endl;
}
template par_t<d3_array,dmatrix,dvector,double>::par_t(const par_t<d3_array,   dmatrix,    dvector,    double>& t);
template par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t(const par_t<d3_array,   dmatrix,    dvector,    double>& t);

/** copy constructor #2.
\callergraph
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t(const par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& t)
: Model_area( t ),
  m_ipar(t.m_ipar), //cohort_index(t.cohort_index),  
  fleet_names(t.fleet_names),start_date(t.start_date),//tr(t.tr),
  afm_yr(t.afm_yr),afm_mo(t.afm_mo),afm_fl(t.afm_fl),afm_i(t.afm_i),
  afm_j(t.afm_j)
{
  cout << "Top of par_t copy constructor #2" << endl;
  cerr << "Top of par_t copy constructor #2" << endl;
  cerr << "This method should never be used???" << endl;
  exit(1);
  /*
  nmonth = t.nmonth;
  nrelease =  t.nrelease;
  nfleet= t.nfleet;

  //tr.ptr = t.tr.ptr;
  tr.ptr = new tagreleasetype_vector(*(t.tr.ptr));

  mort =  value(t.mort);
  special_mort = value(t.special_mort);
  q.allocate(1,nfleet);
  q = value(t.q);
  report_rates.allocate(1,nfleet);
  report_rates = value(t.report_rates);
  nb_par.allocate(1,nfleet);
  nb_par = value(t.nb_par);

  num_afm=t.num_afm;
  if (num_afm)
  {
    minafm = t.minafm;
    maxafm = t.maxafm;
    afm.allocate(1,num_afm);
    afm = value(t.afm);
  }

  minsus_spd = t.minsus_spd;
  maxsus_spd = t.maxsus_spd;
  minMin_D = t.minMin_D;
  maxMin_D = t.maxMin_D;
  minslope = t.minslope;
  maxslope = t.maxslope;
  mininflection = t.mininflection;
  maxinflection = t.maxinflection;

  minsig = t.minsig;
  maxsig = t.maxsig;
  minmort = t.minmort;
  maxmort = t.maxmort;
  minq = t.minq;
  maxq = t.maxq;
  minr_rates = t. minr_rates;
  maxr_rates = t.maxr_rates;
  minvel = t.minvel;
  maxvel = t.maxvel;

  fpen = value(t.fpen);
  total_uv_penalty = t.total_uv_penalty;
  total_sigma_penalty = t.total_sigma_penalty;
  total_negative_tag_penalty = t.total_negative_tag_penalty;
  total_total_tag_penalty = t.total_total_tag_penalty;
  total_bounds_penalty = t.total_bounds_penalty;
  total_likelihood_contribution= t.total_likelihood_contribution;
  objective_function_value = t.objective_function_value;
  nvar = t. nvar;
 
  cout << "Bottom of par_t copy constructor #2" << endl;
  */  
}
template par_t<d3_array,dmatrix,dvector,double>::par_t(const par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& t);
template par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t(const par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& t);


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::~par_t()
{
  //cout << "Top of par_t destructor tr.ptr = 0x" << hex << tr.ptr << dec << endl;
  if (tr.ptr != NULL)
  {
    delete tr.ptr;
    tr.ptr = NULL;
  }

  //cout << "Bottom of par_t destructor tr.ptr = 0x" << hex << tr.ptr << dec << endl;
}
template par_t<d3_array,dmatrix,dvector,double>::~par_t();
template par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::~par_t();

/*
void par_t::dfparkill()
{
  // only usergrid is uniquely allocated in dfparam
  // set to null but don't deallocate; destructor does that
  //			     we need these pointers alive in param
  tr.ptr       = NULL;
  usergrid.ptr = NULL;

}
*/
#include "arrays.cpp"

implement(vector, tagreleasetype_);
// -------> MOVED to recap_t.cpp
//implement(vector, recaptype_);
