//$Id   $
/** \file tagmove15.cpp
main for diffusion simulator.
Sets global variables, exit function and calls tagmove
\ingroup sims
*/
#include <fvar.hpp>
#include <adstring.hpp>
#include <fvar.hpp>
#include <cifstrem.h>
#include <time.h>
#include <jni.h>

#include "par_t_reg.h"
#include "par_t_nn.h"
#include "fish_rec.h"
#include "trace.h"
template <typename D3_ARRAY,typename MATRIX,typename VECTOR,typename DOUBLE>
void readrecaps(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path, indexed_regional_fishery_record& irfr);

#ifdef __APPLE__
template class par_t<d3_array,dmatrix,dvector,double>;
template class par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
#endif

#ifdef __TURBOC__
  #undef __ZTC__
  extern unsigned _stklen = 20000;
#endif

#ifdef __ZTC__
  unsigned _stack = 64000;
#endif

setsaveinterflag interflag;
int saveinterflag = MM_intersave::NONE;

indexed_regional_fishery_record global_irfr;


adstring make_banner(const adstring& program_name);
adstring banner = make_banner("tagmove: tag movement simulator");
ofstream clogf;

int tagmove(JNIEnv* env, jobject jobj, adstring& path, adstring& file);

int _global_report_flag;
#ifdef __CYGWIN32__
void ad_comm::get_slave_assignments(){HERE} // does nothing; satisfies reference
#endif
extern "C"  {
  void ad_boundf(int i)
  {
    // so we can stop here
    exit(i);
  }
} 
/**
\ingroup sims
main() for the tagmove
*/
int main(void)
{
  ad_exit=&ad_boundf;
  JNIEnv* env = NULL;
  jobject jobj = NULL;
#ifdef _WIN32
  tagmove(env, jobj, ".\\", "");
#else
  adstring ads = "./";
  adstring ads2 = "";
  tagmove(env, jobj, ads, ads2);
#endif

}

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

#if defined (__REGIONAL__)
  par_t_reg<d3_array,dmatrix,dvector,double> param(filename);
#elif defined (__NEURAL_NET__)
  par_t_nn<d3_array,dmatrix,dvector,double> param(filename);
#elif defined (__HABITAT__)
  par_t_hab<d3_array,dmatrix,dvector,double> param(filename);
#else
  #error Error: Undefined par_t derived class
#endif

  TRACE(param.m_ipar[13]);
  //HERE
  //recaptype::set_cohort_pooling(param.m_ipar[13]);
  TRACE(recaps.cohort_pooling())
  recaps.set_cohort_pooling(param.m_ipar[13]);
  TRACE(recaps.cohort_pooling())

#ifdef _WIN32
  global_irfr.read(file, path + "..\\");
  readrecaps(param, file, recaps, nrec, path + "..\\",global_irfr);
#else
  global_irfr.read(file, path + "../");
  readrecaps(param, file, recaps, nrec, path + "../",global_irfr);
#endif
  /*
  ivector effortless = recaps.count_effortless_recaps(param.fleet_names,);
  clogf << "Recaptures WITHOUT fishing effort: " << effortless(0) << endl;
  clogf << "Recaptures  with   fishing effort: " << effortless(1) << endl;
  recaps.tabulate_effortless_recaps(clogf,param.fleet_names);
  */
  year_month final_date = param.tr[param.nrelease].date + param.nmonth;

  #ifdef OLD_DO_KLUDGE
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
  #else // new DO effort kludge
  for (int f = 1; f <= param.nfleet; f++)
  {
     if (param.DomesticFleetKludgeFlag(f))
     {
        int nsub =  global_irfr.set_effort_to_recaps(param.fleet_names(f), recaps, nrec, param);
        cout << nsub << " effort records substituted for " << param.fleet_names(f) << endl;
        clogf << nsub << " effort records substituted for " << param.fleet_names(f) << endl;
     }
  }
  #endif
  global_irfr.set_fleet_usage_table(param.fleet_names);
  for (int f = 1; f <= param.nfleet; f++)
  {
     clogf << "fleet name: " << param.fleet_names[f] << ", index " << f 
           << ", ifr index: " << global_irfr.flindex(param.fleet_names[f]) << endl;
  }

  if (param.m_ipar[13] == 2)
    param.collapse_cohorts(recaps, nrec);

  param.set_fishing_effort_normalization(global_irfr);

#ifdef USE_EFFORT_STREAM
  effort_stream::set_irfr_ptr(&global_irfr);

#endif //USE_EFFORT_STREAM
  param.savefit(0.0, 0, 0.0, ".ppp");


  //interflag.setsaveinter(0);

  param.simulate(env, jobj, recaps, nrec, path, file);

  time(&time_sec);
  clogf << "finished time: " << ctime(&time_sec) << endl;
  cout << "finished time: " << ctime(&time_sec) << endl;
  return 0;
}
