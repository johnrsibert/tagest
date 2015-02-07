#include <iostream.h>
#include <fstream.h>
#include <fvar.hpp>
#include <adstring.hpp>
#include <cifstrem.h>
#include <time.h>
#include <jni.h>

#include "par_treg.h"
#include "par_t_nn.h"
#include "fish_rec.h"
#include "trace.h"

#ifdef __CYGWIN32__
void ad_comm::get_slave_assignments(){HERE} // does nothing; satisfies reference
#endif
 

void readrecaps(par_t& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path = DEFAULT_PATH);

#ifdef __TURBOC__
  #undef __ZTC__
  extern unsigned _stklen = 20000;
#endif

#ifdef __ZTC__
  unsigned _stack = 64000;
#endif


setsaveinterflag interflag;
indexed_regional_fishery_record global_irfr;


adstring make_banner(const adstring& program_name);
adstring banner = make_banner("tagmove: tag movement simulator");
ofstream clogf;
#ifdef unix
  uistream ui_wscf("/home/xeon2/msadam/NN_malskip/binwscurl.wsc");
#else
  uistream ui_wscf("..\\binwscurl.wsc");
#endif

//#ifdef __MAKE_EXECUTABLE__

int tagmove(JNIEnv* env, jobject jobj, adstring& path, adstring& file);

int _global_report_flag;

int main(void)
{
  JNIEnv* env = NULL;
  jobject jobj = NULL;
  adstring file("");
#ifdef unix
  //tagmove(env, jobj, "./", "");
  adstring path("./");
#else
  //tagmove(env, jobj, ".\\", "");
  adstring path(".\\");
#endif
  tagmove(env, jobj, path, file);

}

//#endif


int tagmove(JNIEnv* env, jobject jobj, adstring& path, adstring& file)
{
  clogf.open("tagmove.log");
  clogf << "In tagmove(JNIEnv*,jobject)" << endl;
  clogf << banner << endl;
#if defined (__REGIONAL__)
  clogf << "Regional parameterisation." << endl;
#elif defined (__NEURAL_NET__)
  clogf << "Neural net parameterisation." << endl;
#elif defined (__HABITAT__)
  clogf << "Habitat parameterisation." << endl;
#else
  clogf << "Unknown parameterisation." << endl;
#endif

  time_t time_sec;
  time(&time_sec);
  clogf << "starting time: " << ctime(&time_sec);
  cout << "starting time: " << ctime(&time_sec) << endl;

//#ifdef __MAKE_EXECUTABLE__
  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Error: Cannot open file_nam.tmp";
    exit(1);
  }
  char charFilename[9];

  fnt >> charFilename;
  file = adstring(charFilename);
  fnt.close();
//#endif

  HERE

  const int MAXRETURNS = 82000;     // increase for more tag returns
  int  nrec = MAXRETURNS-1;

  recaptype_vector recaps(1, MAXRETURNS);
  //adstring filename = path + file; // + ".par";
  adstring filename =  file;  
  
  HERE

#if defined (__REGIONAL__)
  par_t_reg param(filename);
#elif defined (__NEURAL_NET__)
  par_t_nn param(filename);
#elif defined (__HABITAT__)
  par_t_hab param(filename);
#else
  #error Error: Undefined par_t derived class
#endif

  //HERE
  //recaptype::set_cohort_pooling(param.m_ipar[13]);
  TRACE(recaps.cohort_pooling())
  recaps.set_cohort_pooling(param.m_ipar[13]);
  TRACE(recaps.cohort_pooling())
  //HERE

#ifdef unix
  readrecaps(param, file, recaps, nrec, path + "../");
  global_irfr.read(file, path + "../");
#else
  readrecaps(param, file, recaps, nrec, path + "..\\");
  global_irfr.read(file, path + "..\\");
  //int m = param.get_m();
  //int n = param.get_n();
  //TTRACE(file,charFilename)
  //global_irfr = monthly_average_ifr(charFilename, m, n, "..\\");
#endif
  HERE

  if (param.m_ipar[43])
  {
    adstring phdo("phdo");
    int s_phdo = global_irfr.set_effort_to_recaps(phdo, recaps, nrec, param);
    cout << s_phdo << " effort records substituted for " << phdo << endl;
    clogf << s_phdo << " effort records substituted for " << phdo << endl;
  }

  if (param.m_ipar[44])
  {
    adstring iddo("iddo");
    int s_iddo = global_irfr.set_effort_to_recaps(iddo, recaps, nrec, param);
    cout << s_iddo << " effort records substituted for " << iddo << endl;
    clogf << s_iddo << " effort records substituted for " << iddo << endl;
  }
  HERE

#ifdef __MAKE_EXECUTABLE__
  //param.m_ipar[68] = 1; //jca 2/26/2002 2:03PM fix graphics!
#endif  
  if (param.m_ipar[13] == 2)
    param.collapse_cohorts(recaps, nrec);
  HERE
  param.set_fishing_effort_normalization(global_irfr);
  HERE
  effort_stream::set_irfr_ptr(&global_irfr);
  HERE
  param.savefit(0.0, 0, 0.0, ".ppp");
  HERE

  interflag.setsaveinter(0);

  param.simulate(env, jobj, recaps, nrec, path, file);

  time(&time_sec);
  clogf << "finished time: " << ctime(&time_sec) << endl;
  cout << "finished time: " << ctime(&time_sec) << endl;
  return 0;
}
