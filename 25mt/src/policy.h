//$Id: policy.h 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "ipar.h"
#include "par_t.h"

#ifndef __POLICY_H__
#define __POLICY_H__

// forward declarations
extern template class par_t<d3_array,dmatrix,dvector,double>;
class indexed_regional_fishery_record;
class year_month;

class policy
{
  private:
    void set_switch_names(ipar_t& t);

  public:
    policy (const par_t<d3_array,dmatrix,dvector,double>& param, const char* file_name);

    imatrix zonemap;
    dmatrix zone_multiplier;  // fishing effort multiplier for each zone
    dmatrix fleet_multiplier; // fishing effort multiplier for each fleet
    ivector phase_duration;   // duration of phase in months
    dvector phase_amplification; // factor by which effort grows during phase
    int n;
    int m;
    int nzone;               // number of management zones
    int nphase;              // number of fishery development phases
    int nfleet;              // number of managed fishing fleets
    dvector q;               // catchability coefficient by fleet
    adstring_array  fleet_names;  //vector of char[4] fleet identifiers

    friend class par_t<d3_array,dmatrix,dvector,double>;
    void get_effort_array(par_t<d3_array,dmatrix,dvector,double>& param, indexed_regional_fishery_record& irfr,
             d3_array& effort, year_month& date, ivector& effort_occured,
             const int current_phase, const int month);
    void fish_mort_comp(d3_array& f_mort, const d3_array& effort);
    void write_gmt_zones(Geo_coord& sw_coord);


};

#endif //ifndef __POLICY_H__
