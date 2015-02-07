//$Id: relpmang3.cpp 3167 2014-05-30 02:17:05Z eunjung $
/** \file relpmang3.cpp
Functions to manage tag release information.
*/

#include <iostream>
#include <sstream>

#include "preproc3.h"
#include "yr_mon2.h"
#include "longtude.h"
#include "latitude.h"
#include "modlarea.h"
#include "shift.h"
#include "trace.h"

//extern ofstream clogf;
extern int valid_rec; //number of valid recapture from parproc3.cpp
extern int all_recaps; //number of all recapture form parproc3.cpp


/** 
Determines whether tag release is valid.
Check whether release is in model domain and whether it is on land.
If it is on land it attempts shift the release position to a nearby cell.
\param[in] rel_record adstring containing the input tag releas record
\param par_param par_t instance
\param[out] release_record tagreleasetype_ containing the tag release information
\param[out] ls shift potition list
\param[out] valid_rel int containing 1 if release is in the water and in the model domain; zero otherwise.
*/
void Release_manager::rel_par_work3(adstring& rel_record, par_t<d3_array,dmatrix,dvector,double>& par_param,
       tagreleasetype_& release_record, List<Shift>& ls, int& valid_rel)
{
  cout << "Release: " << cur_release;
  //istrstream in_out((char*)rel_record);
  std::istringstream in_out((char*)rel_record);

  year_month rel_date;
  Longitude rel_long;
  Latitude rel_lat;
  int releases;

  // Determine Model Domain Boundaries
  in_out >> rel_date >> rel_lat >> rel_long >> releases;
  int ri = par_param.long_to_index(rel_long);
  int rj = par_param.lat_to_index(rel_lat);
  int _m = par_param.get_m();
  int _n = par_param.get_n();
  if (rel_date.get_year() < 100)
  {
    TRACE(rel_date)
    rel_date.set(rel_date.get_year()+1900,rel_date.get_month_value());
    TRACE(rel_date)
  }

  release_record.date.set(rel_date.get_year(), rel_date.get_month_value());
  release_record.i = ri;
  release_record.j = rj;
  release_record.tags = releases;
  //release_record.tags = valid_rec;
  release_record.cohort = cur_release;
  release_record.release_cohort = cur_release;
  release_record.cur_rel = cur_release;
  // Is release in the model domain?
  if ((ri > 0) && (rj > 0) && (ri <= _m) && (rj <= _n))
  {
    cout << " is in the model domain";
    /*
    int k = par_param.get_gridmap(ri, rj);

    // Is release on land?
    if (k == 0)
    */
    if (par_param.is_land(rel_long,rel_lat)) 
    {
      cout << "\nRelease " << rel_long << " " << rel_lat;
      cout << " on land; needs to be shifted." << endl;
      adstring message = "Land tag release: ";
      clogf << message << rel_record << endl;
      cout << message << rel_record << endl;
      par_param.shift_position(message, rel_long, rel_lat, ls);
      ri = par_param.long_to_index(rel_long);
      rj = par_param.lat_to_index(rel_lat);
      release_record.i = ri;
      release_record.j = rj;
    }
    par_param.set_tr(cur_release, release_record);

    valid_rel = 1;
  }// if in Model Domain
  else
  {
    cout << "  Not in the model domain; release ignored.";
    valid_rel = 0;
  }
  cout << endl;
}

