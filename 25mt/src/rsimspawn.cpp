//$Id: rsimspawn.cpp 2800 2011-03-01 01:16:59Z jsibert $
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strstream>
#include <errno.h>

#include "par_t_reg.h"
#include "trace.h"

int saveinterflag = 1;
adstring banner;
setsaveinterflag interflag;
int _global_report_flag = 0;
ofstream clogf;

int my_system(const char* command);

/**
\file rsimspawn.cpp 
\ingroup sims

Simulation executive process.
Automates simulation - estimation procedure for regional parameterization.
Assumes:
\li 1. simulation parameters ("The Truth") are in the q00 file.
\li 2. model structure and initial parameter values are in the p00 file.
\li 3. rtagmove and rtagest are on the PATH

The .par and the .tag files are overwritten.

Running in background with nohup is highly recommended.
*/


int main(int argc, char *argv[])
{
  if (argc < 2)
  {
     cerr << "Usage: nohup rsimspan N R&" << endl;
     cerr << "       N is number of simulations and\n"
          << "       R is initial random number seed." << endl;
     exit(1);
  }
  using std::ostrstream;
  const int SeedIncrement = 2;
  clogf.open("simspawn.log");
  const int nsim = atoi(argv[1]);
  TRACE(nsim)
  const int seed = atoi(argv[2]);
  TRACE(seed)

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

  // par_t instance for simulations
  adstring SimName = root + adstring(".q00");
  TRACE(SimName)
  par_t_reg<d3_array,dmatrix,dvector,double> SimPar(SimName);

  // set seed for first simulation
  SimPar.m_ipar(40) = seed;

  // surpress graphics
  SimPar.m_ipar(68) = 0;

  // par_t instance for initial parameter estimates
  adstring InitName = root +adstring(".p00");
  TRACE(InitName)
  par_t_reg<d3_array,dmatrix,dvector,double> InitPar(InitName);

  for (int sim = 1; sim <= nsim; sim++)
  {
    SimPar.savefit(0.0, 0, 0.0, ".par");
    int tagmove_r = my_system("rtagmove");
    clogf << "Finished tagmove: with seed = " << SimPar.m_ipar(40) 
          << "; return code = " << tagmove_r << endl;
 
    char copy_command[80];

    // copy simulated tag file
    ostrstream cpt(copy_command,80);
    cpt << "cp simulate.tag ../" << root << ".tag" << ends;
    TRACE(copy_command)
    my_system(copy_command);

    // set up par file for estimation; overwite simulation version
    InitPar.m_ipar(40) = SimPar.m_ipar(40);
    InitPar.savefit(0.0, 0, 0.0, ".par");

    // run the estimator
    my_system("nice tagest -est");

    // update the seed for the next simulation
    SimPar.m_ipar(40) += SeedIncrement;
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
