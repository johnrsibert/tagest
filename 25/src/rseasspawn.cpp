//$Id: rseasspawn.cpp 3066 2013-04-03 19:54:07Z jsibert $
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strstream>
#include <errno.h>

#include "par_t_reg.h"
#include "trace.h"

using namespace std;

int saveinterflag = 1;
adstring banner;
setsaveinterflag interflag;
int _global_report_flag = 0;
ofstream clogf;

int my_system(const char* command);

/**
\file

Season permutation executive process.
Automates  estimation procedure for selecting optimal seasons.
Assumes:
\li 1. basic model structure is in the q00 file.
\li 2. seasons are of equal length and repeat
\li 3. tagest is on the PATH

The .par and file is overwritten.

Running in background with nohup is highly recommended.
*/


int main(int argc, char *argv[])
{
  if (argc < 2)
  {
     cerr << "Usage: nohup rseasspan n l&" << endl;
     cerr << "       n is number of season and\n"
          << "       l is number of months per season." << endl;
     exit(1);
  }
  clogf.open("seasepawn.log");
  const int nseason = atoi(argv[1]);
  TRACE(nseason)
  const int smonth = atoi(argv[2]);
  TRACE(smonth)

  const int nmodel = 12 / nseason + 1;
  TRACE(nmodel)

  // get root name for files
  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Error: Cannot open file_nam.tmp";
    exit(1);
  }
  adstring root;
  fnt >> root;
  fnt.close();
  TRACE(root)

  adstring SeasName = root + adstring(".q00");
  TRACE(SeasName)
  par_t_reg<d3_array,dmatrix,dvector,double> SeasPar(SeasName);
  SeasPar.nseason = nseason;
  int month1 = 0;

  for (int mod = 1; mod <= nmodel; mod++)
  {
    month1 ++;
    for (int s = 1; s <= SeasPar.nseason; s++)
    {
       SeasPar.startmonth[s] = (month1+(s-1)*smonth);
       if (SeasPar.startmonth[s] > 12)
          SeasPar.startmonth[s]  = SeasPar.startmonth[s] % 12;
    }
    TTRACE(mod,SeasPar.startmonth)
       
    
    SeasPar.savefit(0.0, 0, 0.0, ".par");

    // run the estimator
    my_system("nice tagest");

  }
}

int my_system(const char* command)
{
  errno = 0;
  int system_result = system(command);
  if ( (system_result) && (errno) )
  {
    cerr << command;
    perror(": produced an error");
    cerr << system_result << "  " << errno << endl;
    if ( system_result != 256)
      exit(1);
  }
  cout << "Executed '" << command << "'" << endl;
  return(system_result);
}
