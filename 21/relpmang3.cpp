#include <iostream.h>
#ifdef unix
  #include <strstream.h>
#else
  #include <strstrea.h>
#endif

#include "preproc3.h"
#include "yr_mon2.h"
#include "longtude.h"
#include "latitude.h"
#include "modlarea.h"
#include "shift.h"
#include "trace.h"

extern ofstream clogf;

//int move_corner(const int x, const int edge);
int shift_position(adstring& message, Longitude& i_long, 
                   Latitude& i_lat, List<Shift>& ls);

void Release_manager::rel_par_work3(adstring& rel_record, par_t& par_param,
       tagreleasetype_& release_record, List<Shift>& ls, int& valid_rel)
{
  cout << "Release: " << cur_release;
  istrstream in_out((char*)rel_record);

  year_month rel_date;
  Longitude rel_long;
  Latitude rel_lat;
  int releases;

  // Determine Model Area Boundaries
  in_out >> rel_date >> rel_lat >> rel_long >> releases;
  int ri = par_param.long_to_index(rel_long);
  int rj = par_param.lat_to_index(rel_lat);
  int _m = par_param.get_m();
  int _n = par_param.get_n();

  release_record.date.set(rel_date.get_year(), rel_date.get_month_value());
  release_record.i = ri;
  release_record.j = rj;
  release_record.tags = releases;
  release_record.cohort = cur_release;
  release_record.release_cohort = cur_release;
  release_record.cur_rel = cur_release;
  // Is release in the model area?
  if ((ri > 0) && (rj > 0) && (ri <= _m) && (rj <= _n))
  {
    cout << " is in the model area";
    int k = par_param.get_gridmap(ri, rj);

    // Is release on land?
    if (k == 0)
    {
      cout << "\nRelease " << rel_long << " " << rel_lat;
      cout << " on land; needs to be shifted." << endl;
      adstring mess = "This is yar";
      shift_position(mess, rel_long, rel_lat, ls);
      ri = par_param.long_to_index(rel_long);
      rj = par_param.lat_to_index(rel_lat);
      release_record.i = ri;
      release_record.j = rj;
    }
    par_param.set_tr(cur_release, release_record);

    valid_rel = 1;
  }// if in Model Area
  else
  {
    cout << " Not in the model area; release ignored.";
    valid_rel = 0;
  }
  cout << endl;
}

