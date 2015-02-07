//$Id: walk2.cpp 2829 2011-07-16 03:34:08Z jsibert $
/** \file walk2.cpp
main() random walk tag movement simulator with seasonal variation.
\ingroup sims
*/
#include <iostream>
#include <fstream>
#include <fvar.hpp>
#include <adstring.hpp>
#include <cifstrem.h>
#include <time.h>

#include "par_t_reg.h"
#include "fish_rec.h"
#include "trace.h"
void readrecaps(par_t<d3_array, dmatrix, dvector, double>& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path = DEFAULT_PATH);

adstring make_banner(const adstring& program_name);
adstring banner = make_banner("walk: biased random walk tag movement simulator");
ofstream clogf;

/*
#ifdef __MAKE_EXECUTABLE__
void wormwalk(void);
void main()
{
  wormwalk();
}
#endif

void wormwalk()
*/

int _global_report_flag = 0;
int saveinterflag = 0;

extern "C"  {
  setsaveinterflag interflag;
  void ad_boundf(int i)
  {
    // so we can stop here
    exit(i);
  }
} 

int main()
{
  ad_exit=&ad_boundf;
  clogf.open("walk.log");
  if ( !clogf )
  {
    cerr << "Cannot open file walk.log.\n";
    exit(1);
  }
  clogf << banner << endl;
  cout << banner << endl;
  time_t time_sec;
  time(&time_sec);
  clogf << "starting time: " << ctime(&time_sec);
  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_name.tmp" << endl;
    exit(1);
  }
  char buff[9];
  fnt >> buff;
  fnt.close();
  TRACE(buff)
  char filename[9];
  strcpy(filename,buff);
  TRACE(filename)
  par_t_reg<d3_array, dmatrix, dvector, double> param(filename);
  param.walk();
  time(&time_sec);
  clogf << "finished time: " << ctime(&time_sec) << endl;
	return 0;
}
