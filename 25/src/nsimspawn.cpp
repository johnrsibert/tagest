//$Id: nsimspawn.cpp 2842 2011-08-15 22:35:14Z jsibert $
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include "trace.h"

#if defined(__BCPLUSPLUS__)
  using std::errno;
#else
  extern int errno;
#endif

void make_ext(const char c, const int n, char * ext);
int last_ext_number(const char* root, const char letter);
int my_system(const char* command);

/*
Purpose: To automate a simulation / estimation routine for the neural net implementaion
of the movemod model.  Should have files grid10.p00 and grid10.q00 in the directory where
the  former has the  `true' parameters (used for simulation) and the latter has parameter estimates
removed. ntagest25.exe runs on grid10.q00 taking diffrent seed, i.e., a different starting
values for neural_net parametes

 For the grid10.p00 have the string 'simulate' inserted at line 127 and for the grid10.q00
 have the string 'seed' inserted at lines 483 and/or 535 . This is for awking
*/


void main(void)
{

   char copy_command[80];

   //grid10.p00 shoud have the string "simualte" inserted at line 127
   strcpy(copy_command, "awk -f seed_simulator.awk  grid10.p00 > grid10.zzz ");
   my_system(copy_command);
   cout << copy_command << endl;

   //swap the grid10.p00 with the new simulator seed value
   strcpy(copy_command, "cp grid10.zzz grid10.p00");
   my_system(copy_command);
   cout << copy_command << endl;
   cout << "\finished creating simulator_seed: " << endl;

   //swap file to prepare for simulator
   strcpy(copy_command, "cp grid10.p00 grid10.par");
   my_system(copy_command);
   cout << copy_command << endl;

   //inoke simulator
   int tagmove_r = my_system("ntagmove25");
   cout << "finished tagmove: " << tagmove_r << endl;

   //move simulated tag recoveries to the directory
   int move_r = my_system("cp -f  simulate.tag ../grid10.tag");
   cout << "finished move tag file: "  << move_r << endl;

   strcpy(copy_command, "awk -f seed_bump.awk  grid10.q00 > grid10.par ");
   my_system(copy_command);
   cout << copy_command << endl;

   //save grid10.q00 for next round
   strcpy(copy_command, "cp -f grid10.par grid10.q00");
   my_system(copy_command);
   cout << copy_command << endl;

   int tagest_r = my_system("ntagest25");
   cout << "finished tagest: " << tagest_r << endl;

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
