//$Id: wrtag.cpp 2848 2011-08-25 17:54:33Z jsibert $
/** \file wrtag.cpp
Functions for producing recapture file.
*/
#include <iostream>
#include <fvar.hpp>
#include <adstring.hpp>
#include "par_t.h"
#include "longtude.h"
#include "latitude.h"
#include "fish_rec.h"
#include "trace.h"
#include "recap_t.h"

int effort_nearby(const int ii, const int jj, const dmatrix& effort);
double get_nearby_effort(int& ii, int& jj, const dmatrix& effort);
adstring& adstring_utils_tolower(adstring& ads);

/** Write recaptue file.
Checks that recaptures are in model area, are attributable to valid fleet,
that effort occured by that fleet at the reported positioni, and that
the reported position is not on land.
If no effort at reported position or the recapture is on land,
function trys to shift to a nearby position.
Lots of messages in log, release and par files.
\param rootPath adstring containing path to data directory.
\param tagDataFilename adstring containing the file name of the tag data file.
\param param reference to an instance of type par_t.
\param recaps recapt_type vector that will contain the valid recaptures.
\param maxr integer containing the length of recaps.
*/
void wrtag(const adstring& rootPath,
           const adstring& tag_data_filename,
           par_t<d3_array,dmatrix,dvector,double>& param,
           recaptype_vector& recaps,
           const int maxr)
{
  adstring tag_filename = adstring(param.pathname.getPathRoot()) + ".tag";
  ofstream cf((char*)tag_filename);
  if (cf.bad())
  {
    cerr << "Error: Cannot open " << tag_filename << " for output! " << endl;
    return;
  }

  indexed_regional_fishery_record ifr(param.pathname.getPathRoot(), "");
  int m = param.get_m();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();
  dmatrix effort(1, m, jlb, jub);

  // Write header //////////////////////////////////////////
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

  // Write recaptures //////////////////////////////////////
  for (int k = 1; k <= maxr; k++)
  {
    if ((recaps[k].cohort > 0) && (strlen(recaps[k].fleet) > 0))
    {
      year_month date = recaps[k].date;
      effort.initialize();
      int effort_occured = 0;
      recaps[k].write_on(cf,param); cf << endl;
      nwr ++;
      recaps[k].write_on(clogf,param); clogf << endl;
      recaps[k].write_on(cout,param); cout << endl;

      #undef NEARBY_EFFORT
      #ifdef NEARBY_EFFORT
      int i = recaps[k].i;
      int j = recaps[k].j;
      if (date <= ifr.last_date())
      {
        adstring fleet(recaps[k].fleet);
        adstring_utils_tolower(fleet);
        effort_occured = ifr.get_effort(fleet,date,effort);
      }
      if (param.get_gridmap(i, j) == 0 || effort(i,j) <= 0.0)
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

          get_nearby_effort(i, j, effort);
          recaps[k].i = i;
          recaps[k].j = j;
          cf << "# recapture position shifted to following nearby position:" << endl;
          //cf << "#ps ";
          cf << "    ";
          recaps[k].write_on(cf,param);
          cf << endl;
          nwr ++;

          clogf << "   moved to ";
          recaps[k].write_on(clogf,param);
          clogf << endl;
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
        cf << endl;
        nwr ++;
      }
      //recaps[k].write_on(cout,param); cout << endl;
      #endif // NEARBY_EFFORT
    }
  }
  cf.close();
  cout << "Wrote " << nwr << " records to " << tag_filename << endl;
  clogf << "Wrote " << nwr << " records to " << tag_filename << endl;
}//End wrtag
