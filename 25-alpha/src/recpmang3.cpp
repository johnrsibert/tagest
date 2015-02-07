//$Id: recpmang3.cpp 3166 2014-05-21 21:41:09Z eunjung $
/** \file recpmang3.cpp
Functions to manage tag recapture information.
*/
#include <iostream>
#include <string>
#include <fvar.hpp>
#include <adstring.hpp>

#include "preproc3.h"
#include "yr_mon2.h"
#include "longtude.h"
#include "latitude.h"
#include "par_t.h"
#include "shift.h"
#include "trace.h"
#include "fish_rec.h"
#include "stdio.h" //ej
#include "ctype.h" //ej

//extern ofstream clogf;
extern indexed_regional_fishery_record global_ifr; //object ifr from MakePar.cpp


const int CH_SIZE = 8;
const int FILE_SIZE = 9;
const int G_SIZE = 2;
const int F_SIZE = 2;
const int F_G_SIZE = 4;
int invalid1_recaps = 0; //number of ignored recaptures by missing info
int invalid2_recaps = 0; //number of ignored recaptures by outside of domain
int invalid3_recaps = 0; //number of ignored recaptures by no effort

int cap_rec_ok(year_month & cap_date, Latitude & cap_lat,
	       Longitude & cap_long, adstring & cap_gear,
	       adstring & cap_fleet, adstring & cap_fleet_gear,
	       adstring & tag_number, adstring & rec_record);

/**
Determines whether tag recapture is valid.
Check whether recapture is in model area and whether it is on land.
If it is on land it attempts shift the recapture position to a nearby cell.
\param[in] rec_record adstring containing the input tag releas record
\param par_param par_t instance
\param[out] release_record tag releasetype_ containing the tag release information; used to assign
the release cohort to the recapture.:w

\param[out] ls shift potition list
\param[out] recaps recaptype_vector into which the recapture record is stored.
*/
int Recapture_manager::rec_par_work3(adstring & rec_record,
				      par_t < d3_array, dmatrix, dvector,
				      double >&par_param,
				      tagreleasetype_ & release_record,
				      List < Shift > &ls,
				      recaptype_vector & recaps)
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
			 cap_fleet_gear,	//fleets_tmp,
			 tag_number, rec_record);
   if (cap_date.get_year() < 100)
   {
      cap_date.set(cap_date.get_year() + 1900, cap_date.get_month_value());
   }

   cout << "Recapture: " << cur_recapture << " date: " << cap_date
      << ", tag number " << tag_number << endl;
   clogf << "Recapture: " << cur_recapture << ", stat: " << stat <<
      ", date: " << cap_date << ", tag number " << tag_number << " " <<
      cap_fleet << cap_gear << " " << cap_lat << " " << cap_long << endl;

   // The structure of the following nested if statements:
   // if no fields missing in record
   //   if fleet code is in list
   //     if in the model area
   //       if on land
   //       if check effort
   //       else no effort
   //       else not on land
   //     else not in model area
   //   else fleet code not in list
   // else field is missing in record

   // If record is not missing a field.
   if (stat)
   {
      // Check if the fleet code is in the fleet list
      //long start = 0L;
      int found = FALSE;
      int nfleet = par_param.get_nfleet();
      if (cap_fleet == "NA")
	 cap_fleet_gear = "NANA";
      else if (cap_gear == "OT")
	 cap_fleet_gear = "NANA";
      else
	 cap_fleet_gear = cap_fleet + cap_gear;	// fleet code
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
	 const int icaps = 1;	// number of recaptures per record

	 // Determine if recapture is in the Model Domain
	 int _m = par_param.get_m();
	 int _n = par_param.get_n();
	 // switch to model domain coordinates
	 int ci = par_param.long_to_index(cap_long);
	 int cj = par_param.lat_to_index(cap_lat);
	 if ((ci == 35) && (cj == 15))
	 {
	    clogf << cur_recapture << ": " << rec_record << ", mask: "
	       << par_param.get_gridmap(ci, cj) << endl;
            //global_ifr.read(root_filename,"./");
            //ddreadrecaps(param, root_filename, recaps, nrec, "", global_ifr);

	 }
	 if ((ci > 0) && (cj > 0) && (ci <= _m) && (cj <= _n))  // in Model domain
	 {
	    // Determine if recapture is on land
	    /*
	       #undef USE_EDGE_TEST
	       #ifdef USE_EDGE_TEST
	       union // Anonymous union
	       {
	       unsigned short int boundary;
	       struct
	       {
	       char x;
	       char y;
	       } edge;
	       };
	       boundary = par_param.get_cell_type(ci, cj);
	       if ((edge.x == ISLAND) || (edge.y == ISLAND))
	       #else
	       if (par_param.get_gridmap(ci,cj) == 0)
	       #endif
	     */
	    if (par_param.is_land(cap_long, cap_lat))
	    {
	       cout << "\nRecapture " << dec << cap_long << " " << cap_lat
		  << " on land; needs to be shifted." << endl;
	       clogf << "\nRecapture " << dec << cap_long << " " << cap_lat
		  << " on land; needs to be shifted." << endl;
	       adstring message = "Land tag recapture: ";
	       cout << message << rec_record << endl;
	       clogf << message << rec_record << endl;
	       par_param.shift_position(message, cap_long, cap_lat, ls);
	       ci = par_param.long_to_index(cap_long);
	       cj = par_param.lat_to_index(cap_lat);
	    }
 
            dmatrix t (1,_m,1,_n);
            cap_fleet_gear.to_lower();  //make lower case in fleet_gear
            int fl=global_ifr.flindex(cap_fleet_gear);
            t.initialize();
            int eff = global_ifr.get_effort(fl,cap_date.get_year(),cap_date.get_month_value(),t);
            cout << "eff: " << t(ci,cj) << endl;
            cap_fleet_gear.to_upper(); //make back to uppder case in fleet_gear
            cout << "cap_fleet_gear: " << cap_fleet_gear << endl;
            if (t(ci,cj) > 0)
	    {
               // Add recapture record to recaps
	       recaptype recapture_record;
	       strcpy(recapture_record.fleet, cap_fleet_gear);
	       recapture_record.date.set(cap_date.get_year(),
	   			         cap_date.get_month_value());
	       recapture_record.cohort = release_record.cur_rel;
	       recapture_record.i = ci;
	       recapture_record.j = cj;
	       recapture_record.returns = icaps;
	       recaps[cur_recapture] = recapture_record;
	       clogf <<  cur_recapture << ": " << recapture_record << endl;
               
	       if (icaps == 210)
	       {
	          clogf << cur_recapture << ": " << recaps[cur_recapture] <<
		     ", mask: " << par_param.
		     get_gridmap(recaps[cur_recapture].i,
		   	      recaps[cur_recapture].j) << endl;

	       }
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
            }  //if effort >0 
            else
            {
               invalid3_recaps++;
               cout << "\nNo effort at " << cap_long << " " << cap_lat << "; recapture ignored." << endl;
               clogf << "\nNo effort at " << cap_long << " " << cap_lat << "; recapture ignored." <<  endl;
            }  // else effort >0 
	 }			//if in Model Domain
	 else
	 {
            invalid2_recaps++;
	    cout <<
	       " Recapture is not in the Model Domain; recapture ignored."
	       << endl;
	    clogf <<
	       " Recapture is not in the Model Domain; recapture ignored."
	       << endl;
	 }
      }				// if found fleet code
      else
      {
         invalid2_recaps++;
	 cout << " Fleet code " << cap_fleet_gear << " not in fleet list;"
	    << " recapture ignored. " << endl;
	 clogf << " Fleet code " << cap_fleet_gear << " not in fleet list;"
	    << " recapture ignored. " << endl;
      }
   }				// if stat has no fields missing
   else
   {
      invalid1_recaps++;
      cout << " Field missing in recapture record; recapture ignored. " <<
	 endl;
      clogf << " Field missing in recapture record; recapture ignored. " <<
	 endl;
   }
   return stat;
}				//End rec_par_work3
