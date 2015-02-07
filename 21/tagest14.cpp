#include "precomp.h"
#include "intersav.h"
#include "factoral.h"
#include "fish_rec.h"
#include "trace.h"

#if defined(__BORLANDC__) && !defined(_Windows)
  extern unsigned _stklen = 0x4000;
#endif

#ifdef __ZTC__
  unsigned _stack = 64000;
#endif

void ad_comm::get_slave_assignments(){HERE} // does nothing; satisfies reference
 
int _global_report_flag = 0;

//double fmortcut = 0.95;
int saveinterflag = 1;

double fsave, dfstep, dsave, deriv;
int iswitch = 0;

void readrecaps(par_t& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path = DEFAULT_PATH);

adstring make_banner(const adstring& program_name);
int surpress_core_dump(void);

//double sscale = 0.00001;

ofstream clogf;
#ifdef unix
  uistream ui_wscf("../binwscurl.wsc");
#else
  uistream ui_wscf("..\\binwscurl.wsc");
#endif

adstring banner;
factorial_class FACT(100);
setsaveinterflag interflag;

#undef HERE
#define HERE clogf << "reached " << __LINE__ << " in " << __FILE__ << endl;
#define CLOGF_TRACE(object) clogf << "line " << __LINE__ << ", file " << __FILE__ << ", " << #object " = " << object << endl;

int main(int argc, char ** argv)
{
  banner = make_banner(adstring(argv[0])+": tag movement estimation procedure\n");
#ifdef OPT_LIB
  banner += adstring("\n(Optimized library)");
#else
  banner += adstring("\n(Safe library)");
#endif

  time_t time_sec;
  cout << banner << endl;

  adstring logname("tagest.log");
  clogf.open(logname);
  if ( !clogf )
  {
    cerr << "Cannot open file " << logname << endl;
    exit(1);
  }
  cout << "Opened " << logname << endl;
  //cerr = clogf;
  clogf << banner << endl;
  time(&time_sec);
  time_t time1 = time_sec;
  clogf << "starting time: " << ctime(&time_sec) << endl;

#ifdef unix //__SUNPRO_CC
  //surpress_core_dump();
#endif

  interflag.setsaveinter(1);  /* if 1 psi saving is activated */
  cout << "finished setsaveinterflag(1)" << endl;

  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_nam.tmp in main()" << endl;
    exit(1);
  }
  char filename[9];
  fnt >> filename;
  fnt.close();
  clogf << "Read '" << filename << "' from file_nam.tmp" << endl;
  cout << "Read '" << filename << "' from file_nam.tmp" << endl;

#ifdef DOS386
  const int MAXRETURNS = 12000;     // increase for more tag returns
#else
  const int MAXRETURNS = 2500;     // less than 1 segment
#endif
  int  nrec = MAXRETURNS-1; /* used as check in readrecaps */
  recaptype_vector recaps(1, MAXRETURNS);

#if defined (__REGIONAL__)
  par_t_reg param(filename);
#elif defined (__NEURAL_NET__)
  par_t_nn param(filename);
#elif defined (__HABITAT__)
  par_t_hab param(filename);
#else
  #error Undefined par_t derived class
#endif
  TRACE(recaps.cohort_pooling())
  recaps.set_cohort_pooling(param.m_ipar[13]);
  TRACE(recaps.cohort_pooling())

  Upwind_coff_t coff(param, param.m_ipar[6]);
  coff.clean();
  Upwind_coff_t dfcoff(param, param.m_ipar[6]);
  dfcoff.clean();

  readrecaps(param, filename, recaps, nrec);
  if (nrec <=0)
  {
     cerr << "No recapture data found." << endl;
     exit(1);
  }   

  indexed_regional_fishery_record irfr(filename);

  if (param.m_ipar[43])
  {
    adstring phdo("phdo");
    int s_phdo = irfr.set_effort_to_recaps(phdo, recaps, nrec, param);
    cout << s_phdo << " effort records substituted for " << phdo << endl;
    clogf << s_phdo << " effort records substituted for " << phdo << endl;
  }

  if (param.m_ipar[44])
  {
    adstring iddo("iddo");
    int s_iddo = irfr.set_effort_to_recaps(iddo, recaps, nrec, param);
    cout << s_iddo << " effort records substituted for " << iddo << endl;
    clogf << s_iddo << " effort records substituted for " << iddo << endl;
  }
  HERE
  
  param.set_fishing_effort_normalization(irfr);

  param.savefit(0.0, 0, 0.0, ".ppp");

  if (param.m_ipar[13] == 2)
  {
    param.collapse_cohorts(recaps, nrec);
    //param.savefit(0.0, 0, 0.0, ".qqq");
    //exit(1);
  }
  
  if (irfr.is_normalized())
    clogf << "Fishing effort is normalized." <<endl;
  else
    clogf << "Fishing effort not normalized." << endl;

  /* calculate number of parameters and allocate x, g and w */
  int nvar = param.nvarcalc();
  cout << "nvar = " << nvar << endl;
  independent_variables x(1, nvar);
  dvector g(1, nvar);
  adstring_array x_names(1,nvar);
  param.xinit(x,x_names);
  //cout << "Returned from xinit" << endl;
  //CLOGF_TRACE(x)
  clogf << "xinit complete" << endl;
  cout << "xinit complete" << endl;

  for (int i = 1; i <= nvar; i++)
  {
    //cout << setw(5) << i << " - " << x_names(i) << " = " << x[i] << endl;
    clogf << setw(5) << i << " - " << x_names(i) << " = " << x[i] << endl;
  }


  fmm fmc(nvar);

  fmc.maxfn  = param.m_ipar[1];
  if (fmc.maxfn <= 0)
    fmc.ireturn = -1;
  fmc.iprint = param.m_ipar[2];
  fmc.crit   = param.m_ipar[3]/1000.;
  fmc.imax   = param.m_ipar[4];
#ifdef __ZTC__
  fmc.scroll_flag = 0;
#else 
  fmc.scroll_flag = param.m_ipar[7];
#endif
  fmc.ifn    = 0;

  param.m_ipar[5] = 0;// Number of calls to fgcomp
  double like = 0.0;
  ios::sync_with_stdio();

  cout << "entering minimization loop" << endl;
  while (fmc.ireturn >= 0)
  {
    fmc.fmin(like, x, g);
    //cout << "finished call "<< fmc.ireturn << " to fmin" << endl;
    {
      if (fmc.ireturn > 0)
      {
        //cout << "calling fgcomp" << endl;
        param.fgcomp(like, x, g, nvar, recaps, nrec, coff, dfcoff, irfr);
        //cout << "finished call "<< fmc.ireturn << " to fgcomp" << endl;
        if (fmc.ireturn < 3)
        {
          param.m_ipar[5] ++;
          if ((param.m_ipar[5] % 10) == 0)
          {
            double gmax = max(fabs(g));
            clogf << "Saving temporary parameter file for function evaluation "
                  << param.m_ipar[5] << endl;
            param.savefit(like, nvar, gmax, ".tmp");
          }  
        }
      }
    }
  }
  cout << endl << "\nFinished fmin loop" << endl;

  _global_report_flag = 1;
  clogf << "\nGlobal report flag is now on." << endl;
  param.fgcomp(like, x, g, nvar, recaps, nrec, coff, dfcoff, irfr);

  time(&time_sec);
  clogf << "\nfinished time: " << ctime(&time_sec);// << endl;
  time_t time2 = time_sec;
  double elapsed_time = (double)(time2-time1);
  param.m_ipar[8] =  (time2-time1)/60; //int(elapsed_time/60.0);
  double gmax = max(fabs(g));
  if (param.m_ipar[1]>1)
    param.savefit(like, nvar, gmax);
  clogf << "total time for estimation: " << elapsed_time/60.0
	      << " minutes" << endl;
  clogf.close();

  interflag.setsaveinter(0);  // free buffer
  cout << "Done!" << endl;
  exit(0);
}//End of main
