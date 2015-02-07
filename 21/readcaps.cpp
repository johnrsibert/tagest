#include "precomp2.h"
#include <fstream.h>
#include "par_t.h"
#include "bound.h"
#include "recap_t.h"
//#include "trace.h"
adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);
//int move_corner(const int i, const int edge);

int cohort_released(par_t& p, recaptype_& recapture_record);

extern ofstream clogf;

void readrecaps(par_t& param, const char* root, recaptype_vector& recaps,
    int& pnrecs, const char* path)
{
  bound boundary;
/*
#if defined(__ZTC__) || defined(__BORLANDC__)
  adstring filename = "..\\" + adstring(root) + ".tag\0";
#endif

#if defined(__GNUC__) || defined(__SUN__)
  adstring filename = "../" + adstring(root) + ".tag\0";
#endif

  adstring filename2 = adstring(path) + filename;
*/
  adstring filename = adstring(path) + adstring(root) + ".tag\0";

  cout << "Tag file: " << filename << endl;
  cifstream inf(filename);

  if ( !inf )
  {
    cerr << "Cannot open recapture file: " << filename << endl;
    pnrecs = 0;
    return;
  }

  int total_caps = 0;
  int island_caps = 0;
  int sw_long = param.get_long_lat_long();
  int sw_lat = param.get_long_lat_lat();
  int _m = param.get_m();
  int _n = param.get_n();
  inf.set_ignore_eof();
  recaptype_ recapture_record;

  int nread = 0;
  int k = 0;
  do
  {
    //inf >> recapture_record;
    int   year;     // month of recapture
    int   month;    // month of recapture
    double lon, lat;
    inf >> recapture_record.fleet;
    inf >> year;
    inf >> month;
    inf >> recapture_record.cohort;
    //s >> recapture_record.i;
    inf >> lon;
    //s >> recapture_record.j;
    inf >> lat;
    inf >> recapture_record.returns;
    recapture_record.date.set(year, month);

    //inf >> month; // get first field and test for eof()
  #ifdef __TURBOC__
    if (inf.good())
  #endif
  #ifdef __GNUC__
    if (inf)
  #endif
  #ifdef __SUN__
    if (inf)
  #endif
  #ifdef __ZTC__
    if (inf)
  #endif
  #ifdef __NDPX__
    //int ss = inf.rdstate();
    //cout << ss << "  " << k << endl;
    //ss = inf.eof();
    //cout << ss << "  " << k << endl;
    if (inf)
  #endif
    {
      nread ++;
      int i = param.long_to_index(lon);
      int j = param.lat_to_index(lat);
      recapture_record.i = i;
      recapture_record.j = j;
      
      if ( (i>_m) | (j>_n) || (i<1) || (j<1) )
      {
        clogf << "Recapture position out of range: "
              <<  "longitude = " << recapture_record.i
              <<  ", latitude = " << recapture_record.j
               << ", i = " << i << ",j = " << j << endl;
      }
      else
      {
    if (cohort_released(param, recapture_record))
    {
        k ++;
        strcpy(recaps[k].fleet,  recapture_record.fleet);
        for (int f = 1; f <= param.get_nfleet(); f++)
        {
            adstring fleet = recaps[k].fleet;
          //fleet.to_lower();
            adstring_utils_tolower(fleet);
            if (fleet == param.fleet_names[f])
            {
                recaps[k].flindex = f;
                break;
            }
        }
        recaps[k].date = recapture_record.date;
        if (recaps.cohort_pooling() == 1)
           recaps[k].cohort = 0;
        else
        recaps[k].cohort = recapture_record.cohort;
        recaps[k].i = recapture_record.i;
        recaps[k].j = recapture_record.j;
        recaps[k].returns = recapture_record.returns;
        total_caps += recaps[k].returns;
        boundary = param.get_cell_type(recaps[k].i,recaps[k].j);
        if ((boundary.edgex() == ISLAND) || (boundary.edgey() == ISLAND))
        {
            cerr << "**  Tag recapture record " << k << " is on an island!" << endl;
            clogf << "Tag recapture record " << k << " is on an island!" << endl;
            clogf /* <<  "  " << recapture_record */ << endl;
            island_caps ++;
        }
        //cout << "       recaps[" << k <<"] :" << recaps[k] << endl;
    }//end if(cohort_released
  }  //end-else
    }  //end-if(inf)
  } while ( inf && (k <=  pnrecs) );

  if (k >= pnrecs)
  {
    cerr << "You are only allowed " << pnrecs << " tag return records. "
        "Stopping there." << endl;
    /* retain input value of pnrecs */
  }
  else
  {
    pnrecs = k;
  }
  inf.close();

  recaps.set_total_recaps(total_caps);
  clogf << nread << " recapture records read" << endl;
  clogf << pnrecs << " recapture records used" << endl;
  clogf << total_caps << " recaptures in data structure" << endl;
  clogf << "Last record:" << endl;
  clogf << "  " << recaps[pnrecs].date << "  " << recaps[pnrecs].cohort
  << "  " << recaps[pnrecs].i << "  " << recaps[pnrecs].j
  << "  " << recaps[pnrecs].returns << endl;

  cout  << nread << " recapture records read" << endl;
  cout  << pnrecs << " recapture records used" << endl;
  cout  << total_caps << " recaptures in data structure" << endl;
  cout  << "Last record:" << endl;
  cout  << "  " << recaps[pnrecs].date << "  " << recaps[pnrecs].cohort
  << "  " << recaps[pnrecs].i << "  " << recaps[pnrecs].j
  << "  " << recaps[pnrecs].returns << endl;

  if (island_caps)
  {
     cerr << island_caps << " recaptures on islands" << endl;
     clogf << island_caps << " recaptures on islands" << endl;
  }
} /* end of read recaps */
