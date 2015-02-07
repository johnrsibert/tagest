#include <iostream.h>
#include <fvar.hpp>
#include "fish_rec.h"
#include "par_t.h"
#include "trace.h"

extern ofstream clogf;

void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr,
             d3_array& effort, year_month& date, ivector& effort_occured)
{
  effort.initialize();
  effort_occured.initialize();

  for (int fl = 1; fl <= param.get_nfleet(); fl++)
  {
     effort_occured(fl) = irfr.get_effort(param.fleet_names(fl),
                                          date, effort(fl));
     //clogf << "effi_v: " << date << " " << param.fleet_names(fl) << " "
     //      << fl << " " << sum(effort(fl)) << endl;
  }
}//End of get_effort_array

void get_average_effort_array(par_t& param, indexed_regional_fishery_record& irfr,
             d3_array& effort, year_month& date, ivector& effort_occured)
{
   year_month adate(1,date.get_month_value());
   get_effort_array(param, irfr, effort, adate, effort_occured);
} //End of get_average_effort_array

