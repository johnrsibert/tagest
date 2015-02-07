//$Id: ttac.cpp 3163 2014-04-26 01:58:02Z eunjung $
/** 
\file ttac.cpp. Code to generate .csv files for creating tag attirtion 
and related plots. Distinguish between recaptures with effort and
recaptures without effort.
Supercedes tac.cpp. 
<b> The output is in a different format:</b> 

For pooling flag (ipar[13]) 0 and 2 the output is
\verbatim
"observed tag rcaptures from .tag file with pooling flag = 0"
"month","with.effort","effortless","total"
0,2397,924,3321
1,2424,534,2958
2,970,381,1351
\endverbatim

For pooling flag 1 (monthly cohorts) the output is
\verbatim
"observed tag rcaptures from .tag file with pooling flag = 1"
"month","YYYY","MM","with.effort","effortless","total"
0,2006,8,142,1,143
1,2006,9,607,2,609
2,2006,10,799,0,799
\endverbatim
*/
#include "intersav.h"
#include "recap_t.h"
#include "fish_rec.h"
#include "trace.h"
#include <prnstrem.h>
#include <sstream>

#ifdef __TURBOC__
#undef __ZTC__
extern unsigned _stklen = 0x4000;
#endif

#ifdef __ZTC__
unsigned _stack = 64000;
#endif

#ifdef __APPLE__
template class par_t <d3_array, dmatrix, dvector, double>;
template class par_t <dvar3_array, dvar_matrix, dvar_vector, dvariable>;
#endif

dvector displacement_bins(par_t < d3_array, dmatrix, dvector,
                    double >&param, dmatrix & distances);
void displacement_frequency(recaptype_vector & recaps, int nrecaps,
                      par_t < d3_array, dmatrix, dvector,
                      double >&param, dmatrix & displacements,
                      dmatrix & xdisp, dmatrix & ydisp,
                      const dmatrix & distances,
                      const dvector & bins);

void write_displacement_files(const dmatrix & displacement,
                        dmatrix & xdisp, dmatrix & ydisp,
                        const dvector & bins,
                        const int fit_number, const char op);


int _global_report_flag = 0;
int saveinterflag = 1;
setsaveinterflag interflag;
ofstream clogf("ttac.log");
adstring make_banner(const adstring & program_name);
adstring banner;
extern "C"
{
   void ad_boundf(int i)
   {
      // so we can stop here
      exit(i);
   }
}


int main(int argc, char **argv)
{
   ad_exit = &ad_boundf;

   banner =
      make_banner(adstring(argv[0]) +
              ": observed tag attrition data generator");
   cout << banner << endl;
   clogf << banner << endl;
   cifstream fnt("file_nam.tmp");
   if (!fnt)
   {
      cerr << "Cannot open file_name.tmp in main()" << endl;
      exit(1);
   }
   char filename[20];
   fnt >> filename;
   fnt.close();
   cout << "Read '" << filename << "' from file_nam.tmp" << endl;


   par_t < d3_array, dmatrix, dvector, double >::createInstance(filename);
   if (par_t < d3_array, dmatrix, dvector, double >::instance == 0)
   {
      exit(1);
   }
   par_t < d3_array, dmatrix, dvector, double >&param =
      *par_t < d3_array, dmatrix, dvector, double >::instance;


   indexed_regional_fishery_record irfr(filename);

   if (recaptype_vector::createInstance
       (filename, *par_t < d3_array, dmatrix, dvector, double >::instance,
      irfr) == 0)
   {
      exit(1);
   }
   recaptype_vector & recaps = *recaptype_vector::instance;
   const int nrec = recaps.getTotalRecords();
   if (nrec <= 0)
   {
      cerr << "Error: No recapture data found." << endl;
   }

   if (param.m_ipar[13] == 2)
   {
      param.collapse_cohorts(recaps, nrec);
   }

   ivector effortless = recaps.count_effortless_recaps();
   clogf << "\nRecaptures WITHOUT fishing effort: " << effortless(0) << endl;
   clogf << "Recaptures  with   fishing effort: " << effortless(1) << endl;
   clogf << "Total recaptures:                  " << effortless(2) << endl;
   recaps.tabulate_effortless_recaps(clogf,param.fleet_names);

   const int ncolumns = 3;
   const int ncohort = param.nrelease;
   const int pool_tags = param.m_ipar[13];


   int max_cohort_number = 0;
   for (int k = 1; k <= ncohort; k++)
   {
       if (param.tr[k].cohort > max_cohort_number)
           max_cohort_number = param.tr[k].cohort;
   } 
   TTRACE(ncohort,max_cohort_number)
   year_month_vector release_dates(0,max_cohort_number);
   ivector is_released(0,max_cohort_number);
   is_released.initialize();
   for (int k = 1; k <= ncohort; k++)
   {
       int cc = param.tr[k].cohort;
       is_released(cc) = 1;
       release_dates(cc) = param.tr[k].date;
   } 
   clogf << "\nRelease cohorts:" << endl;
   is_released(0) = 1;
   release_dates(0) = param.start_date;
   TRACE(max_cohort_number)
   for (int cc = 0; cc <= max_cohort_number; cc++)
   {
       clogf << cc << " " << is_released(cc) ;
       clogf       << " " << release_dates(cc) << endl;
   }
   int max_month = param.get_tr_date(param.get_nrelease())
      - param.get_tr_date(1) + param.get_nmonth() + 1;
   TRACE(max_month)
   TRACE(ncolumns)
   dmatrix rtac(0, max_month + 1, 1, ncolumns);
   rtac.initialize();
  
   TRACE(nrec)//ej
   for (int k = 1; k <= nrec; k++)
   {
       year_month date = recaps[k].date;
       int cc = 0;
       int tal = 0;
       if (pool_tags == 0)
           cc = recaps[k].cohort;
       else if (pool_tags == 1)
           cc = 0;
       else if (pool_tags == 2)
           cc = recaps[k].cohort;

       TTRACE(cc,date)
       TTRACE(recaps[k].date, release_dates(cc))
       tal = recaps[k].date - release_dates(cc);
       TRACE(tal) //ej
       if (tal > max_month)
          tal = max_month +1;
       if (tal < 0)// by ej to prevent error from recapture date error
       {  tal = 0;
          cout << "recap date is earlier than release date" << endl;
       }
       if (is_released(cc))
       {
           if (recaps[k].effort_flag == 1)   // recapture with effort
              rtac(tal,1) += recaps[k].returns;
           else                              // effortless recaptures
              rtac(tal,2) += recaps[k].returns;

          rtac(tal,3) += recaps[k].returns;  // total recaptures
       }
       TTRACE(nrec,k) //ej
   }

   ostringstream prn_name;
   prn_name << "fit" << setw(3) << setfill('0') << hex << param.m_ipar[8]
      << "_P" << pool_tags << "_ota.csv" << '\0';
   prnstream prn(prn_name.str().c_str());
   clogf << "open observed attrition file " << prn_name.str() << endl;

   ostringstream header;
   header << "observed tag rcaptures from .tag file with pooling flag = "
          << pool_tags;
   prn << header.str().c_str() << endl;
   
   prn << "month";
   if (pool_tags == 1)
       prn << "YYYY" << "MM";
   prn << "with.effort" << "effortless" << "total" << endl;

   year_month date = param.start_date;
   for (int tal = 0; tal <= max_month + 1; tal++)
   {
      //ostrstream sdate;
      //sdate << date;
      prn << tal;
      if (pool_tags == 1)
      {
         //prn << sdate.str();
         //sdate.clear();
         prn << date.get_year() << date.get_month_value();
      }
      for (int column = 1; column <= ncolumns; column++)
         prn << rtac(tal, column);
      prn << endl;
      clogf << tal << " " << rtac(tal) << endl;
      date ++;
   }
   clogf << "sums:" << colsum(rtac) << endl;
   prn.close();
   
   if (pool_tags == 0)
   {
      const int m = param.get_m();
      const int n = param.get_n();
      dmatrix distances(0, m, 0, n);
      dvector dbins = displacement_bins(param, distances);
      dmatrix displacement(0, max_month + 1, 0, dbins.indexmax() + 1);
      dmatrix xdisp(0, max_month + 1, -(m + 1), m + 1);
      dmatrix ydisp(0, max_month + 1, -(n + 1), n + 1);

      displacement_frequency(recaps, nrec, param, displacement, xdisp,
                             ydisp, distances, dbins);
      write_displacement_files(displacement, xdisp, ydisp, dbins,
                               param.m_ipar[8], 'o');
   }
  
   return 0;
}
