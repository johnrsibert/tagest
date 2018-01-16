#include <adstring.hpp>
#include "preproc3.h"
#include "fish_rec.h"
#include "shift.h"

void eff_process(adstring& path,
                 adstring& root_filename,
                 adstring& date_filename,
                 adstring& fleet_filename,
                 par_t& eff_param)
{
  // Names of all the fleets in the Model.
  adstring fleet_names = adstring(fleet_filename) + ".dat";
  ifstream fleets(fleet_names);
  if ( !fleets)
  {
      cerr << "Cannot open " << fleet_names << " in effort processor\n";
      exit(1);
  }
  fleets >> eff_param.fleet_names;
  if ( !fleets )
  {
    cerr << "Error reading fleet_names from " << fleet_names;
    exit(1);
  }
  //cout << "Fleets " << eff_param.fleet_names << endl;
  fleets.close();

  adstring date_list = adstring(date_filename) + ".dat";
  ifstream dates(date_list);
  if ( !dates )
  {
    cerr << "Error opening " << date_list << endl;
    exit(1);
  }
  int start_year, start_month;
  int end_year, end_month;
  dates >> start_year >> start_month;
  dates >> end_year >> end_month;
  if ( !dates )
  {
    cerr << "Error reading starting and ending dates from "
         << date_list << endl;
    exit(1);
  }
  dates.close();

  // Holds all the shifted coordinates
  List<Shift> lse;

  indexed_regional_fishery_record ifr(path, eff_param, start_year, end_year, lse);
  ifr.write(root_filename);
}//End eff_process
