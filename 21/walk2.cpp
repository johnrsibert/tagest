// random walk tag movement simulator with seasonal variation
#include <iostream.h>
#include <fstream.h>
#include <fvar.hpp>
#include <cifstrem.h>
#include <adstring.hpp>
#include <time.h>

#include "par_treg.h"
#include "fish_rec.h"
#include "trace.h"
void readrecaps(par_t& param, const char* root, recaptype_vector& recaps,
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
int main(void)
{
  clogf.open("walk.log");
  if ( !clogf )
  {
    cerr << "Cannot open file walk.log.\n";
    exit(1);
  }
  clogf << banner << endl;
  HERE
  cout << banner << endl;
  HERE
  time_t time_sec;
  HERE
  time(&time_sec);
  HERE
  clogf << "starting time: " << ctime(&time_sec);
  HERE
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
  par_t_reg param(filename);

  HERE
  param.walk();
  HERE
  time(&time_sec);
  clogf << "finished time: " << ctime(&time_sec) << endl;

} // end of main
