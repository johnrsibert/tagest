//$Id: fadregion.cpp 2894 2011-12-02 01:07:19Z jsibert $
/** \file fadregion.cpp
Process FAD set data tp compute sums by region.
\ingroup preproc
*/
#include <fvar.hpp>
#include <adstring.hpp>
#include <cifstrem.h>
#include "par_t_reg.h"
#include "recap_t.h"
#include "trace.h"
#include "prnstrem.h"
ofstream clogf;
adstring make_banner(const adstring& program_name);
int _global_report_flag;
template <typename D3_ARRAY,typename MATRIX,typename VECTOR,typename DOUBLE>
void readrecaps(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path, indexed_regional_fishery_record& irfr);


extern "C"  {
  void ad_boundf(int i)
  {
    // so we can stop here
    exit(i);
  }
} 

int main(int argc, char ** argv)
{
   adstring banner = make_banner(adstring(argv[0])+": region FAD counter.");
   cout << banner << endl;
 
   adstring log_file_name("fadregion.log");
 
   clogf.open(log_file_name);
   if ( !clogf )
   {
     cerr << "Cannot open file " << log_file_name << endl;
     exit(1);
   }
   cout << "Opened " << log_file_name << endl;
   clogf << banner << endl;
 
   char root_filename[50];
   #undef USE_FILE_NAM_TMP
   #ifdef USE_FILE_NAM_TMP
   //Open file_nam.tmp to read names of files for input
   cifstream fnt("file_nam.tmp");
   if ( !fnt)
   {
     cerr << "Error: Cannot open file_nam.tmp";
     exit(1);
   }
   fnt >> root_filename;
   fnt.close();
 
   #else
   if (argc < 2)
   {
      cerr << "Must specifify file name root of .prn file on command line, e.g.\n"
           << "  fadregion skj16" << endl;
      exit(1);
   }
   strcpy(root_filename,argv[1]);
   #endif // USE_FILE_NAM_TMP
 
   adstring pfile = adstring(root_filename);// + adstring(".prn");
   TRACE(pfile)
 
   par_t_reg<d3_array,dmatrix,dvector,double> ma(pfile);
   int nregion = max(ma.gridmap);
   year_month start_date = ma.start_date;
   year_month end_date = ma.tr(ma.nrelease).date;
   end_date += ma.nmonth;
 
   adstring fadfile("/home/jsibert/movemod/SPC2011/FADsets/fadsets.dat");
   cifstream ff(fadfile);
   if (!ff)
   {
      cerr << "Error: Cannot open " << fadfile << endl;
      exit(1);
   }

   int year = 0;
   int mon = 0;
   int day = 0;
   int hour = 0;
   double lon = 0.0;
   double lat = 0.0;
   int id = 0;
   imatrix fads(0,nregion,1,2);
   fads.initialize();

   int fr = 0;
   while (ff)
   {
      ff >> year >> mon >> day >> hour >> lon >> lat >> id;
      year_month set_date(year,mon);
      fr ++;
      if (fr == 1)
      {
         clogf << "First FAD set:" << endl;
         clogf << " " << year << " " << mon << " " << day << " " << hour << " " << lon << " " << lat << " " << id << " " << endl;
      }
      if (set_date >= start_date && set_date <= end_date)
      {
         int i = ma.long_to_index(lon);
         int j = ma.lat_to_index(lat);
         if (i > 0 && i <= ma.m && j > 0 && j <= ma.n)
         {
            int k = ma.gridmap(i,j);
            int type = 0;
            if (id == 5) // anchored
               type = 1;
            else // drifiting
               type = 2;
            fads(k,type) ++;
         }
      }
   }
   clogf << "Last FAD set:" << endl;
   clogf << " " << year << " " << mon << " " << day << " " << hour << " " << lon << " " << lat << " " << id << " " << endl;
   clogf << "Number of FAD records: " << fr << endl;
   cout << "Read " << fr << " FAD records."  << endl;
   clogf << "\nNumber of reported FAD sets by region from "
        << start_date << " to " << end_date << endl;
   clogf << "  k    aFAD    dFAD    All" << endl;
           //  7      65    1575   1640
 
   for (int k = 0; k<=nregion; k++)
   {
      clogf << setw(3) << k << setw(7) << fads(k) << (fads(k,1) + fads(k,2)) << endl;
   }

   csvstream fst("fadregion.csv");
   fst << "region" << "aFAD" << "dFAD" << "All" << endl;
   for (int k = 1; k<=nregion; k++)
   {
      fst << k;
      for (int type = 1; type <= 2; type++)
         fst << fads(k,type);
      fst << (fads(k,1) + fads(k,2)) << endl;
   }
   fst.close();

  
   cout << "Finished." << endl;
}
 
