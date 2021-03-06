//$Id: readcaps.cpp 3009 2012-11-14 22:53:50Z jsibert $
#include "par_t.h"
#include "bound.h"
#include "recap_t.h"
#include "fish_rec.h"
#include "trace.h"

adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
adstring& adstring_utils_toupper(adstring& ads);
adstring_array& adstring_utils_toupper(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

int cohort_released(par_t& p, recaptype_& recapture_record);

extern ofstream clogf;
 
void readrecaps(par_t& param, const char* root, recaptype_vector& recaps, int& pnrecs, const char* path, indexed_regional_fishery_record& irfr)
{
  TTRACE(path,root)
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
    if (inf)
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
          //for (int f = 1; f <= param.get_nfleet(); f++)
          //{
            adstring fleet = recaps[k].fleet;
            adstring_utils_tolower(fleet);
            //if (fleet == param.fleet_names[f])
            //{
            //    TRACE(recaps[k])
            //    recaps[k].flindex = f;
            //    TTRACE(recaps[k], irfr.flindex(adstring_utils_tolower(fleet)))
            //    break;
            //}
            recaps[k].ifr_flindex = irfr.flindex(fleet);
            recaps[k].par_flindex = adstring_utils_index(param.fleet_names,fleet);

          //}
          recaps[k].date = recapture_record.date;
          if (recaps.cohort_pooling() == 1)
           recaps[k].cohort = 0;
          else
            recaps[k].cohort = recapture_record.cohort;
          recaps[k].i = recapture_record.i;
          recaps[k].j = recapture_record.j;
          recaps[k].returns = recapture_record.returns;

          recaps[k].effort_flag = 0;
          fleet = recaps[k].fleet;
          int flindex =  irfr.flindex(adstring_utils_tolower(fleet));
          if (flindex <= 0)
          {
              cerr << "Fleet " << fleet << " not found in correctly indexed; " 
                   << "index = " << flindex << ". Exiting." << endl;
              exit(1);
          }
          int year = recaps[k].date.get_year();  
          int month = recaps[k].date.get_month_value();  
          int n1 = irfr.get_index(flindex,year,month);
          int n2 = n1+irfr.get_nrec(flindex,year,month)-1;
          for (int n = n1; n <= n2; n++)
          {
            regional_fishery_record fr = irfr.get_record(n);
            int ii = fr.get_i();
            int jj = fr.get_j();
            if ( (ii == recaps[k].i) && (jj == recaps[k].j) )
            {
               recaps[k].effort_flag = 1;
               break;
            }
          }
          /*
          if (k == 9)
          {
             clogf << k << ": "  << lon << "x" << lat << ", "<< recaps[k] << ", mask: " 
                   << param.get_gridmap(recaps[k].i,recaps[k].j) << endl;
          }
          */
          total_caps += recaps[k].returns;
        /*
        #undef USE_EDGE_TEST
        #ifdef USE_EDGE_TEST
          bound boundary = param.get_cell_type(recaps[k].i,recaps[k].j);
          if ((boundary.edgex() == ISLAND) || (boundary.edgey() == ISLAND))
        #else
          if (param.get_gridmap(recaps[k].i,recaps[k].j) == 0)
        #endif
        */
          if (param.is_land(lon,lat)) 
          {
            cerr << "**  Tag recapture record " << k << " is on an island!" << endl;
            clogf << "**  Tag recapture record " << k << " is on an island!" << endl;
            clogf << "  " << recaps[k]  << endl;
            clogf << "  " << recapture_record  << endl;
            island_caps ++;
          }
          //cout << "       recaps[" << k <<"] :" << recaps[k] << endl;
        }//end if(cohort_released
      }  //end-else
    }  //end-if(inf)
  } while ( inf && (k <=  pnrecs) ); // do

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
  recaps.set_nrec(pnrecs);
  clogf << nread << " recapture records read" << endl;
  clogf << pnrecs << " recapture records used" << endl;
  clogf << total_caps << " recaptures in data structure" << endl;
  cout  << nread << " recapture records read" << endl;
  cout  << pnrecs << " recapture records used" << endl;
  cout  << total_caps << " recaptures in data structure" << endl;
  if ( (total_caps <= 0) || (pnrecs<=0) )
  {
     cerr << "Problem reading recapture data." << endl;
     ad_exit(1);
  }
  if (pnrecs > 0 )
  {
    clogf << "Last record:" << endl;
    clogf << "  " << recaps[pnrecs].date << "  " << recaps[pnrecs].cohort
          << "  " << recaps[pnrecs].i << "  " << recaps[pnrecs].j
          << "  " << recaps[pnrecs].returns << endl;

    cout  << "Last record:" << endl;
    cout  << "  " << recaps[pnrecs].date << "  " << recaps[pnrecs].cohort
          << "  " << recaps[pnrecs].i << "  " << recaps[pnrecs].j
          << "  " << recaps[pnrecs].returns << endl;
  }
  if (island_caps)
  {
     cerr << island_caps << " recaptures on islands" << endl;
     clogf << island_caps << " recaptures on islands" << endl;
  }
}
 
recaptype_vector* recaptype_vector::createInstance(const adstring &filename, par_t &param, indexed_regional_fishery_record& irfr) 
{
   if (instance == 0)
   {
      const int MAXRETURNS = 12000;     // increase for more tag returns
      int nrec = MAXRETURNS-1; // used as check in readrecaps
      instance = new recaptype_vector(1, MAXRETURNS);
      instance->set_cohort_pooling(param.m_ipar[13]);
#ifdef unix
      readrecaps(param, filename, *instance, nrec, "../", irfr);
#else
      readrecaps(param, filename, *instance, nrec, "..\\", irfr);
#endif
      instance->setTotalRecords(nrec);
   }
   return instance;
}
