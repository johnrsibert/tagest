#pragma implementation "par_t.h"
#include <fstream.h>
#include "par_t.h"
#include "trace.h"
#include "ipar.h"

extern ofstream clogf;
//#undef HERE
//#define HERE clogf << "reached line " << __LINE__ << " in " << __FILE__ << endl;
//extern adstring par_file_version;

//void set_ipar_names(ipar_t& t);

void par_t::allocate()
{
  if (nrelease > 0)
    tr.ptr = new tagreleasetype_vector(1,nrelease);
  else
    cout << "Warning: par_t::nrelease = " << nrelease << endl;

  //cohort_index.allocate(1,nrelease);

  q.allocate(1,nfleet);
  report_rates.allocate(1,nfleet);

  fleet_names.allocate(1,nfleet);

  nb_par.allocate(1,nfleet);
  nb_par.initialize();
  minafm = 0.0;
  maxafm = 10.0;
  if (num_afm)
  {
    //minafm = 0.0;
    //maxafm = 10.0;
    afm_yr.allocate(1,num_afm);
    afm_mo.allocate(1,num_afm);
    afm_fl.allocate(1,num_afm);
    afm_i.allocate(1,num_afm);
    afm_j.allocate(1,num_afm);
    afm.allocate(1,num_afm);
  }

}

void par_t::initialize()
{
  //set_ipar_names(m_ipar);
  total_negative_tag_penalty = 0;
  total_total_tag_penalty = 0;
  total_bounds_penalty = 0;
}

par_t::par_t()
{
  //cout << "Top of par_t null constructor" << endl;
	tr.ptr = NULL;
  //cout << "Bottom of par_t null constructor" << endl;
}

par_t::par_t(const par_t& t)
: Model_area( t ),
  m_ipar(t.m_ipar), //cohort_index(t.cohort_index),  
  fleet_names(t.fleet_names),start_date(t.start_date),//tr(t.tr),
  afm_yr(t.afm_yr),afm_mo(t.afm_mo),afm_fl(t.afm_fl),afm_i(t.afm_i),
  afm_j(t.afm_j)
{
  //cout << "Top of par_t copy constructor" << endl;

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
  fpen = t.fpen;

  total_uv_penalty = t.total_uv_penalty;
  total_sigma_penalty = t.total_sigma_penalty;
  total_negative_tag_penalty = t.total_negative_tag_penalty;
  total_total_tag_penalty = t.total_total_tag_penalty;
  total_bounds_penalty = t.total_bounds_penalty;
  total_likelihood_contribution= t.total_likelihood_contribution;
  objective_function_value = t.objective_function_value;
  nvar = t. nvar;
  /*
  for (int i = 1; i <= nrelease; i++)
  {
    clogf << t.tr[i] << endl;
    clogf << tr[i] << endl << endl;
  }
  */
  //cout << "Bottom of par_t copy constructor" << endl;
}

par_t::~par_t()
{
  //cout << "Top of par_t destructor tr.ptr = 0x" << hex << tr.ptr << dec << endl;
  if (tr.ptr != NULL)
  {
    delete tr.ptr;
    tr.ptr = NULL;
  }

  //cout << "Bottom of par_t destructor tr.ptr = 0x" << hex << tr.ptr << dec << endl;
}

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
