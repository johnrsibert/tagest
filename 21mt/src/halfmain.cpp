#include <iostream>
#include <fstream>
#include <adstring.hpp>
#include <fvar.hpp>
#include <cifstrem.h>
#include <time.h>
#include <jni.h>

#include "par_treg.h"
#include "fish_rec.h"
#include "trace.h"


setsaveinterflag interflag;
indexed_regional_fishery_record global_irfr;

adstring make_banner(const adstring& program_name);
adstring banner = make_banner("halflife: halfilfe computations");
ofstream clogf;
/*
#ifdef __MAKE_EXECUTABLE__

int tagmove(JNIEnv* env, jobject jobj, adstring& path, adstring& file);
int main(void)
{
  JNIEnv* env = NULL;
  jobject jobj = NULL;
#ifdef unix
  tagmove(env, jobj, "./", "");
#else
  tagmove(env, jobj, ".\\", "");
#endif

}

#endif
*/

//int halflife(JNIEnv* env, jobject jobj, adstring& path, adstring& file)
//int halflife(void)
int main(void)
{
  clogf.open("halflife.log");
  clogf << "In halflife(JNIEnv*,jobject)" << endl;
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
  HERE
  par_t_reg param(charFilename);
  HERE

  if ( (param.m_ipar(121) != 1) || (param.m_ipar(122) != 1) )
  {
    cerr << endl
         << "Error: Halflife computations can only be made for fits with normalized\n"
         << "       fishing effort (flag 121) and scaled catchabilities (flag 122).\n"
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

  indexed_regional_fishery_record irfr = monthly_average_ifr(charFilename, m, n, ifrpath);
  HERE
  irfr.tabulate(clogf);
  HERE
  param.set_fishing_effort_normalization(irfr);
  HERE
  //effort_stream::set_irfr_ptr(&irfr);
  //HERE
  param.savefit(0.0, 0, 0.0, ".ppp");
  HERE

  interflag.setsaveinter(0);

#ifdef __MAKE_EXECUTABLE__
  param.m_ipar[68] = 1;
#endif  
  param.halflife(irfr);

  time(&time_sec);
  clogf << "finished time: " << ctime(&time_sec) << endl;
  cout << "finished time: " << ctime(&time_sec) << endl;
  return 0;
}
