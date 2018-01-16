#include <iostream>
#include <string.h>
#include <adstring.hpp>

#include "preproc3.h"
#include "yr_mon2.h"
#include "longtude.h"
#include "latitude.h"
#include "par_t.h"
#include "shift.h"
#include "trace.h"

extern ofstream clogf;

const int CH_SIZE = 8;
const int FILE_SIZE = 9;
const int G_SIZE = 2;
const int F_SIZE = 2;
const int F_G_SIZE = 4;

int cap_rec_ok(year_month& cap_date, Latitude& cap_lat, Longitude& cap_long,
               adstring& cap_gear, adstring& cap_fleet,
               adstring& cap_fleet_gear,
               adstring& tag_number, adstring& rec_record);
int shift_position(adstring& message, Longitude& i_long, Latitude& i_lat,
                   List<Shift>& lsc);


void Recapture_manager::rec_par_work3(adstring& rec_record, par_t& par_param, 
                                      tagreleasetype_& release_record,
                                      //year_month_vector& last_recapture_date,
                                      List<Shift>& ls, recaptype_vector& recaps)
{
  year_month cap_date;
  Latitude cap_lat;
  Longitude cap_long;
  adstring cap_gear(1, G_SIZE);
  adstring cap_fleet(1, F_SIZE);
  adstring cap_fleet_gear(1, F_G_SIZE);
  adstring fleets_tmp(1, FILE_SIZE);
  adstring tag_number(1, CH_SIZE);
  int stat = cap_rec_ok(cap_date, cap_lat, cap_long, cap_gear, cap_fleet,
	                cap_fleet_gear, //fleets_tmp,
                        tag_number, rec_record);
  cout << "Recapture: " << cur_recapture << " date: "<< cap_date
                        << ", tag number " << tag_number << endl;
  clogf << "Recapture: " << cur_recapture << " date: " << cap_date
                        << ", tag number " << tag_number << " "
                        << cap_fleet << cap_gear << " " 
                        << cap_lat << " " << cap_long << endl;

  // The structure of the following nested if statements:
  // if no fields missing in record
  //   if fleet code is in list
  //     if in the model area
  //       if on land
  //       else not on land
  //     else not in model area
  //   else fleet code not in list
  // else field is missing in record

  // If record is not missing a field.
  if ( stat )
  {
    // Check if the fleet code is in the fleet list
    long start = 0L;
    int found = FALSE;
    int nfleet = par_param.get_nfleet();
    cap_fleet_gear = cap_fleet + cap_gear; // fleet code
    for (int h = 1; h <= nfleet; h++)
    {
      adstring temp = par_param.fleet_names[h];
      if (cap_fleet_gear == temp)
      {
        found = TRUE;
        break;
      }
    }

    // If valid fleet code ...
    if (found)
    {
      int icaps = 1;

      // Determine if recapture is in the Model Area
      int _m = par_param.get_m();
      int _n = par_param.get_n();
      int ci = par_param.long_to_index(cap_long);
      int cj = par_param.lat_to_index(cap_lat);
      if ((ci > 0) && (cj > 0) && (ci <= _m) && (cj <= _n))
      {

        union // Anonymous union
        {
          unsigned short int boundary;
          struct
          {
            char x;
            char y;
          } edge;
        };


        // Determine if recapture is on land
        boundary = par_param.get_cell_type(ci, cj);
        if ((edge.x == ISLAND) || (edge.y == ISLAND))
        {
          cout << "\nRecapture " << dec << cap_long << " " << cap_lat
               << " on land; needs to be shifted." << endl;
          adstring mess = "This is the recapture position";
          shift_position(mess, cap_long, cap_lat, ls);
          //int fg = int(cap_long.long_lat_value());
          //int gp = int(cap_lat.long_lat_value());
          //f = fg;
          //g = gp;
        }

        // Add recapture record to recaps
        recaptype recapture_record;
        strcpy(recapture_record.fleet, cap_fleet_gear);
        recapture_record.date.set(cap_date.get_year(),
                                  cap_date.get_month_value());
        recapture_record.cohort = release_record.cur_rel;
        // switch to equator-dateline coordinates
        recapture_record.i = par_param.long_to_index(cap_long); //f;
        recapture_record.j = par_param.lat_to_index(cap_lat); //g;
        recapture_record.returns = icaps; // number of tags??
        recaps[cur_recapture] = recapture_record;

        //cout << "recaps[" << cur_recapture << "] = "
        //     << recapture_record << endl;
        //clogf << "Recapture[" << cur_recapture << "] = "
        //     << recapture_record << endl;

/*
        // update
        int cohort = recapture_record.cohort;
        if (recapture_record.date > last_recapture_date[cohort])
        {
          last_recapture_date[cohort] = recapture_record.date;
        }
*/

      }//if in Model Area
      else
      {
        cout << " Recapture is not in the Model Area; recapture ignored."
             << endl;
        clogf << " Recapture is not in the Model Area; recapture ignored."
              << endl;
      }
    }// if found fleet code
    else
    {
      cout << " Fleet code " << cap_fleet_gear << " not in fleet list;"
           << " recapture ignored. "  << endl;
      clogf << " Fleet code " << cap_fleet_gear << " not in fleet list;"
            << " recapture ignored. "  << endl;
    }
  }// if stat has no fields missing
  else
  {
    cout << " Field missing in recapture record; recapture ignored. " << endl;
    clogf << " Field missing in recapture record; recapture ignored. " << endl;
  }
}//End rec_par_work3
