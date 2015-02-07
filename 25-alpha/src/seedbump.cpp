//$Id: seedbump.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "precomp.h"
#include "intersav.h"
#include "factoral.h"
#include "fish_rec.h"
#include "trace.h"

adstring make_banner(const adstring& program_name);
adstring banner = make_banner("seedbump: tag movement random number seed incrementer");
int saveinterflag = 0;
int _global_report_flag = 0;

ofstream clogf;
/*
#ifdef unix
  uistream ui_wscf("../binwscurl.wsc");
#else
  uistream ui_wscf("..\\binwscurl.wsc");
#endif
*/

void main()
{
  clogf.open("seedbump.log");
  if ( !clogf )
  {
    cerr << "Cannot open file seedbump.log.\n";
    exit(1);
  }
  clogf << banner << endl;

  cout << banner << endl;

  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_name.tmp" << endl;
    exit(1);
  }
  char buff[9];

  fnt >> buff; //  myfgets(buff, 9, dfile);
  fnt.close(); // fclose(dfile);

  char filename[9];
  strcpy(filename,buff);

#if defined (__REGIONAL__)
  par_t_reg param(filename);
#elif defined (__NEURAL_NET__)
  par_t_nn param(filename);
#elif defined (__HABITAT__)
  par_t_hab param(filename);
#else
  #error Undefined par_t derived class
#endif

  param.m_ipar[40] += 2;

  param.savefit(0.0, 0, 0.0, ".par");

} // end of main

