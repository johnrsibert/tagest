//$Id: tagest14.cpp 2826 2011-06-18 01:55:56Z eunjung $
#include "par_t_reg.h"
#include "intersav.h"
#include "factoral.h"
#include "fish_rec.h"
#include "trace.h"
#include "coff_t.h"
#include <time.h>

#if defined(__BORLANDC__) && !defined(_Windows)
  extern unsigned _stklen = 0x4000;
#endif

#ifdef __ZTC__
  unsigned _stack = 64000;
#endif

int _global_report_flag = 0;
#ifdef __CYGWIN32__
//void ad_comm::get_slave_assignments(){HERE} // does nothing; satisfies reference
#endif


//double fmortcut = 0.95;

double fsave, dfstep, dsave, deriv;
int iswitch = 0;

adstring make_banner(const adstring& program_name);

int surpress_core_dump(void);

ofstream clogf;

adstring banner;
setsaveinterflag interflag;
int saveinterflag = MM_intersave::MM_mode;


int tagest(int argc, char ** argv)
{
  gradient_structure gs(1500000L);

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

  interflag.setsaveinter(MM_intersave::MM_mode);  /* if 1 psi saving is activated */
  cout << "finished setsaveinter(MM_intersave::MM_mode)" << endl;

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

	par_t<d3_array,dmatrix,dvector,double>::createInstance(filename);
	if (par_t<d3_array,dmatrix,dvector,double>::instance == 0) {
		return 0;
	}
	par_t<d3_array,dmatrix,dvector,double> &param = *par_t<d3_array,dmatrix,dvector,double>::instance;
	param.executable = adstring(argv[0]);

	//const bool centered = param.m_ipar(49);
	coff_t<d3_array,dmatrix,dvector,double>* coff = coff_t<d3_array,dmatrix,dvector,double>::createInstance(param);
	coff_t<d3_array,dmatrix,dvector,double>* dfcoff = coff_t<d3_array,dmatrix,dvector,double>::createDfInstance(param);
	coff->clean();
	dfcoff->clean();

	//recaptype_vector *recaps = recaptype_vector::createInstance(filename, param);

  indexed_regional_fishery_record irfr(filename);
  recaptype_vector *recaps = recaptype_vector::createInstance(filename, param,irfr);
 
  #ifdef OLD_DO_KLUDGE_
  if (param.m_ipar[43])
  {
    adstring phdo("phdo");
    int s_phdo = irfr.set_effort_to_recaps(phdo, *recaps, recaps->getTotalRecords(), param);
    cout << s_phdo << " effort records substituted for " << phdo << endl;
    clogf << s_phdo << " effort records substituted for " << phdo << endl;
  }
  if (param.m_ipar[44])
  {
    adstring iddo("iddo");
    int s_iddo = irfr.set_effort_to_recaps(iddo, *recaps, recaps->getTotalRecords(), param);
    cout << s_iddo << " effort records substituted for " << iddo << endl;
    clogf << s_iddo << " effort records substituted for " << iddo << endl;
  }
  #else // new DO effort kludge
  for (int f = 1; f <= param.nfleet; f++)
  {
     if (param.DomesticFleetKludgeFlag(f))
     {
        int nsub =  irfr.set_effort_to_recaps(param.fleet_names(f), *recaps, recaps->getTotalRecords(), param);
       cout << nsub << " effort records substituted for " << param.fleet_names(f) << endl;
       clogf << nsub << " effort records substituted for " << param.fleet_names(f) << endl;
     }
  }
  #endif
  HERE

  param.set_fishing_effort_normalization(irfr);

  param.savefit(0.0, 0, 0.0, ".ppp");
  if (param.m_ipar[13] == 2)
  {
    param.collapse_cohorts(*recaps, recaps->getTotalRecords());
    //param.savefit(0.0, 0, 0.0, ".qqq");
    //exit(1);
  }

  if (irfr.is_normalized())
    clogf << "Fishing effort is normalized." <<endl;
  else
    clogf << "Fishing effort not normalized." << endl;

  // calculate number of parameters and allocate x, g and w


  const int nvar = param.nvarcalc();
  cout << "nvar = " << nvar << endl;
	if (nvar <= 0) {
		cerr << "Error: nvar=" << nvar << ", no variables to estimate\n";
		par_t<d3_array,dmatrix,dvector,double>::deleteInstance();
		coff_t<d3_array,dmatrix,dvector,double>::deleteInstance();
		recaptype_vector::deleteInstance();
		return 0;
	}


  independent_variables x(1, nvar);
  dvector g(1, nvar);

  adstring_array x_names(1,nvar);
  param.xinit(x,x_names);
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
  fmc.crit   = param.m_ipar[3]/100.;
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
  //int iprevlike = 0;
  //double prevlike = 0.0;
  
  while (fmc.ireturn >= 0)
  {
    fmc.fmin(like, x, g);

    //clogf << "finished call "<< fmc.ireturn << " to fmin" << endl;
    {
      if (fmc.ireturn > 0)
      {
        //clogf << "calling fgcomp" << endl;
        param.fgcomp(like, x, g, nvar, *recaps, recaps->getTotalRecords(), *coff, *dfcoff, irfr);
/*
	if (fabs(like-prevlike) < 0.01)
	{
          if(iprevlike >= 20)
	  {
            double gmax = max(fabs(g));
	    param.savefit(like,nvar,gmax);
            cout <<"Done! " << endl;
	    exit(1);
	  }
	  else
	   iprevlike ++;
	}
	else
	  iprevlike = 0;
        
	prevlike = like;
*/
	 
        //clogf << "finished call "<< fmc.ireturn << " to fgcomp" << endl;
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

  param.fgcomp(like, x, g, nvar, *recaps, recaps->getTotalRecords(), *coff, *dfcoff, irfr);

  time(&time_sec);
  clogf << "\nfinished time: " << ctime(&time_sec);// << endl;
  time_t time2 = time_sec;
  double elapsed_time = (double)(time2-time1);
  //param.m_ipar[9] = int(elapsed_time/60.0);
  double gmax = max(fabs(g));
  if (param.m_ipar[1]>1)
    param.savefit(like, nvar, gmax, 0);
  clogf << "total time for estimation: " << elapsed_time/60.0
	      << " minutes" << endl;

  clogf.close();

  interflag.setsaveinter(0);  // free buffer
  cout << "Done!" << endl;

	par_t<d3_array,dmatrix,dvector,double>::deleteInstance();
	coff_t<d3_array,dmatrix,dvector,double>::deleteInstance();
	recaptype_vector::deleteInstance();
	
	return 0;
}
