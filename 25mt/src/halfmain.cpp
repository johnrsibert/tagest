//$Id: halfmain.cpp 2955 2012-02-21 01:15:21Z jsibert $
/** \file halfmain.cpp
main for halflife computation
Sets global variables, exit function and calls par_t_reg::halflife(indexed_regional_fishery_record& irf0
\ingroup sims
*/

#include <iostream>
#include <fstream>
#include <fvar.hpp>
#include <adstring.hpp>
#include <cifstrem.h>
#include <time.h>
#include <jni.h>

#include "par_t_reg.h"
#include "fish_rec.h"
#include "trace.h"

int saveinterflag = 0;


setsaveinterflag interflag;
indexed_regional_fishery_record global_irfr;

adstring make_banner(const adstring& program_name);
adstring banner = make_banner("halflife: halfilfe computations");
ofstream clogf;

int _global_report_flag;
#ifdef __CYGWIN32__
void ad_comm::get_slave_assignments(){HERE} // does nothing; satisfies reference
#endif
 

int main(void)
{
  clogf.open("halflife.log");
  clogf << banner << endl;

  time_t time_sec;
  time(&time_sec);
  clogf << "starting time: " << ctime(&time_sec);
  cout << "starting time: " << ctime(&time_sec) << endl;

  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Error: Cannot open file_nam.tmp";
    exit(1);
  }
  char charFilename[9];

  fnt >> charFilename;
  fnt.close();
  adstring filename =  adstring(charFilename);

  HERE
  //par_t_reg param(charFilename);
#if defined (__REGIONAL__)
  par_t_reg<d3_array,dmatrix,dvector,double> param(filename);
#elif defined (__NEURAL_NET__)
  par_t_nn<d3_array,dmatrix,dvector,double> param(filename);
#elif defined (__HABITAT__)
  par_t_hab<d3_array,dmatrix,dvector,double> param(filename);
#else
  #error Error: Undefined par_t derived class
#endif

  HERE

  if ( (param.m_ipar(52) != 1) || (param.m_ipar(52) != 1) )
  {
    cerr << endl
         << "Error: Halflife computations can only be made for fits with normalized\n"
         << "       fishing effort (flag 51) and scaled catchabilities (flag 52).\n"
         << endl;
    exit(1);
  }
 

#ifdef unix
  adstring ifrpath = "../";
#else
  adstring ifrpath = "..\\"; 
#endif
  int m = param.get_m();;
  int n = param.get_n();

  //indexed_regional_fishery_record irfr = monthly_average_ifr(filename, m, n, ifrpath);
  indexed_regional_fishery_record irfr;
  irfr.read(filename,ifrpath);
// = monthly_average_ifr(filename, m, n, ifrpath);
  //irfr.tabulate(clogf);
  HERE
  param.set_fishing_effort_normalization(irfr);
  HERE
  //effort_stream::set_irfr_ptr(&irfr);
  //HERE
  param.savefit(0.0, 0, 0.0, ".ppp");
  HERE

  interflag.setsaveinter(0);

//#ifdef __MAKE_EXECUTABLE__
//param.m_ipar[68] = 1;
//#endif  
  param.halflife(irfr);

  time(&time_sec);
  clogf << "finished time: " << ctime(&time_sec) << endl;
  cout << "finished time: " << ctime(&time_sec) << endl;
  return 0;
}
