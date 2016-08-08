//$Id: halfmain.cpp 2955 2012-02-21 01:15:21Z jsibert $
/** \file halfmain.cpp
main for halflife computation
\ingroup sims
*/

#include <iostream>
#include <fstream>
#include <fvar.hpp>
#include <adstring.hpp>
#include <cifstrem.h>
#include <time.h>
//#include <jni.h>

#include "par_t_reg.h"
#include "fish_rec.h"
#include "trace.h"

int saveinterflag = 0;
setsaveinterflag interflag;

int _global_report_flag;
indexed_regional_fishery_record global_irfr;

adstring make_banner(const adstring & program_name);
adstring banner = make_banner("halflife: halfilfe computations");
ofstream clogf;
extern "C"
{
   void ad_boundf(int i)
   {
      // so we can stop here
      exit(i);
   }
}

/**
\ingroup sims
main() for halflife computation
*/
int main(void)
{
   clogf.open("halflife.log");
   clogf << banner << endl;

   time_t time_sec;
   time(&time_sec);
   clogf << "starting time: " << ctime(&time_sec);
   cout << "starting time: " << ctime(&time_sec) << endl;

   cifstream fnt("file_nam.tmp");
   if (!fnt)
   {
      cerr << "Error: Cannot open file_nam.tmp";
      exit(1);
   }
   char charFilename[9];

   fnt >> charFilename;
   fnt.close();
   adstring filename = adstring(charFilename);

   HERE
#if defined (__REGIONAL__)
   par_t_reg < d3_array, dmatrix, dvector, double >param(filename);
#elif defined (__NEURAL_NET__)
   par_t_nn < d3_array, dmatrix, dvector, double >param(filename);
#elif defined (__HABITAT__)
   par_t_hab < d3_array, dmatrix, dvector, double >param(filename);
#else
#error Error: Undefined par_t derived class
#endif
   TRACE(param.m_ipar(52))
   if ((param.m_ipar(52) != 1) || (param.m_ipar(52) != 1))
   {
      cerr << "Error: Halflife computations can only be made for fits with normalized\n"
	 "       fishing effort (flag 51) and scaled catchabilities (flag 52).\n"
	 << endl;
      exit(1);
   }

#ifdef unix
   adstring ifrpath = "../";
#else
   adstring ifrpath = "..\\";
#endif
   global_irfr.read(charFilename, ifrpath);

   //global_irfr.tabulate(clogf);
   param.set_fishing_effort_normalization(global_irfr);
   param.savefit(0.0, 0, 0.0, ".ppp");
   //interflag.setsaveinter(0);

   param.halflife(global_irfr);
   time(&time_sec);
   clogf << "finished time: " << ctime(&time_sec) << endl;
   cout << "finished time: " << ctime(&time_sec) << endl;
   return 0;
}
