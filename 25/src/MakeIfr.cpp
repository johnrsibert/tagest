//$Id: MakeIfr.cpp 3164 2014-05-08 21:29:52Z eunjung $
#include <fvar.hpp>
#include <adstring.hpp>
#include "intersav.h"
#include "fish_rec.h"
/** \file MakeIfr.cpp 
Utility to create indexed regional fishery record data structure.
Creates initializes global variables.
\ingroup preproc
*/
ofstream clogf;
void rpreproc_main(char c, const adstring& path);
adstring banner;
int _global_report_flag = 0;
int saveinterflag = 1;
setsaveinterflag interflag;
extern "C"  {
  void ad_boundf(int i)
  {
    // so we can stop here
    exit(i);
  }
} 

indexed_regional_fishery_record global_ifr;

/**
\ingroup preproc
main() for the makeifr application.
Sets sets exit procudure and calls  rpreproc_main('e', "").
*/
int main(void)
{
  ad_exit=&ad_boundf;
  clogf.open("makeifr.log");
  rpreproc_main('e', "");
  return 0;
}
