#include <iostream>
#include <adstring.hpp>
#include "par_t.h"
#include "longtude.h"
#include "latitude.h"
#include "fish_rec.h"
#include "trace.h"
#include "recap_t.h"

int effort_nearby(const int ii, const int jj, const dmatrix& effort);
double get_nearby_effort(int& ii, int& jj, const dmatrix& effort);

void wrtag(adstring&         rootPath,
           adstring&         tag_data_filename,
           par_t&            param,
           recaptype_vector& recaps,
           const int         maxr)
{
  adstring tag_filename = adstring(param.pathname.getPathRoot()) + ".tag";
  TRACE(tag_filename)
  ofstream cf((char*)tag_filename);
  if (cf.bad())
  {
    cerr << "Error: Cannot open " << tag_filename << " for output! " << endl;
    return;
  }

  indexed_regional_fishery_record ifr(param.pathname.getPathRoot(), "");
  int m = param.get_m();
  int n = param.get_n();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();
  dmatrix effort(1, m, jlb, jub);

  /// Write header //////////////////////////////////////////
  cf << "#\n# tag recaptures generated from " << tag_data_filename
     << "\n#\n# fleets : ";
  int _nfleet = param.get_nfleet();
  int nwr = 0;
  for (int f = 1; f <= _nfleet; f++)
  {
    cf << " " << param.fleet_names[f];
  }
  cf << "\n# southwest corner: " << param.get_sw_coord()
     << "\n#\n#me indicates effortless recapture\n"
     << "#\n#                            recapture\n"
     <<      "#  fleet year month cohort long   lat     recaps"
     << endl;

  /// Write recaptures //////////////////////////////////////
  TRACE(maxr)
  for (int k = 1; k <= maxr; k++)
  {
    if (k > 19188)
    {
      recaps[k].write_on(clogf,param); 
      clogf << endl;
    }
    if ((recaps[k].cohort > 0) && (strlen(recaps[k].fleet) > 0))
    {
      year_month date = recaps[k].date;
      adstring fleet(recaps[k].fleet);
      fleet.to_lower();
      int i = recaps[k].i;
      int j = recaps[k].j;
      effort.initialize();
      int effort_occured = 0;
      //recaps[k].write_on(clogf,param); clogf << endl;
      if (date <= ifr.last_date())
        effort_occured = ifr.get_effort(fleet,date,effort);
      if (effort(i,j) <= 0.0)
      {
        //cf << "#me "<< recaps[k] << "\n";
        cf << "#me ";
        recaps[k].write_on(cf,param);
        cf << endl;
        nwr ++;

        int nearby = effort_nearby(i, j, effort);
        if (nearby)
        {
          //clogf << "\nme recapture" << recaps[k] << endl;
          clogf << "\nme recapture";
          recaps[k].write_on(clogf,param);
          clogf << endl;

          double nearby_effort = get_nearby_effort(i, j, effort);
          recaps[k].i = i; //param.int_to_long(i); //a_move_corner(i,sw_long);
          recaps[k].j = j; //param.int_to_lat(j); //a_move_corner(j,sw_lat);
          cf << "# recapture position shifted to following nearby position:" << endl;
          //cf << "#ps " << recaps[k] << "\n";
          cf << "#ps ";
          recaps[k].write_on(cf,param);
          cf << endl;
          nwr ++;

          //clogf << "   moved to " << recaps[k] << endl;
          clogf << "   moved to ";
          recaps[k].write_on(clogf,param);
          clogf << endl;
          TRACE(k)
        }
        else
        {
          cf << "# no effort in nearby positions" << endl;
        }

      }
      else
      {
        //cf << recaps[k] << "\n";
        recaps[k].write_on(cf,param);
        TRACE(k)
        cf << endl;
        nwr ++;

      }
      //recaps[k].write_on(cout,param); cout << endl;
    }
  }
  HERE
  cf.close();
  TTRACE(k,nwr)
  cout << "Wrote " << nwr << " records to " << tag_filename << endl;
  clogf << "Wrote " << nwr << " records to " << tag_filename << endl;
}//End wrtag
