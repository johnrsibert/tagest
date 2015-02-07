#include "precomp.h"
#include "fish_rec.h"
#include "intersav.h"
#include "factoral.h"
#include "trace.h"
ostream& setfixed(ostream& _s);
ostream& setscientific(ostream& _s);

int saveinterflag = 1;
int _global_report_flag = 0;

void readrecaps(par_t& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path = DEFAULT_PATH);
int extra_nvarcalc(par_t_reg& param);
void extra_names(par_t_reg& param,adstring_array& name, const int nvar);
void sd_scale(dvector& d, dvector& x, par_t_reg& param);
void obrien_derivs(dmatrix& S, dvector& x, dvector& scale,
                   dvector& v, int nvar_model, int nvar_extra,
                   par_t_reg& param);
void hess_inv(par_t& param, int nvar, dvector& x,
              dmatrix& hess, const adstring_array& x_names);
void model_sd(dmatrix& S, dvector& x, dvector& scale, dvector& std_dev,
              int nvar_model);


void hess_comp(par_t_reg& param, int nvar_model, dvector& x,
	    recaptype_vector& recaps,
	    int nrec, int npoint,
	    indexed_regional_fishery_record& irfr);
void hess_inv(par_t_reg& param, int nvar, dvector& x, const adstring_array& x_names);
int file_exists(char* name);
adstring make_banner(const adstring& program_name);

ofstream clogf;
adstring banner;
factorial_class FACT(100);
setsaveinterflag interflag;

void main()
{
  time_t time_sec;
  banner = make_banner("covar: parameter covariance computations");
  adstring clogf_name("covar.log");
  cout << banner << endl;

  clogf.open(clogf_name);
  cout << "Opened " << clogf_name << endl;
  clogf << banner << endl;
  time(&time_sec);
  clogf << "starting time: " << ctime(&time_sec) << endl;

  interflag.setsaveinter(1);  /* if 1 psi saving is activated */
  cout << "finished setsaveinterflag(1)" << endl;

  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_name.tmp in main()" << endl;
    exit(1);
  }
  char filename[9];
  //adstring filename(1,9);
  fnt >> filename;
  fnt.close();
  clogf << "Read '" << filename << "' from file_nam.tmp" << endl;
  cout << "Read '" << filename << "' from file_nam.tmp" << endl;


  const int MAXRETURNS = 12000;     /* increase for more tag returns */
  int  nrec = MAXRETURNS-1; /* used as check in readrecaps */
  recaptype_vector recaps(1, MAXRETURNS);

#if defined (__REGIONAL__)
  par_t_reg param(filename);
#elif defined (__NEURAL_NET__)
  par_t_nn param(filename);
#elif defined (__HABITAT__)
  par_t_hab param(filename);
#else
  #error Error: Undefined par_t derived class
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
  HERE

  param.savefit(0.0, 0, 0.0, ".ppp");
  HERE

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



  int nvar_model = param.nvarcalc();
  int nvar_extra = extra_nvarcalc(param);
  int nvar = nvar_model+nvar_extra;
  cout << "nvar = " << nvar << endl;

//dvector x(1, nvar);
  independent_variables x(1, nvar);
  x.initialize();
  dvector g(1, nvar_model);
  g.initialize();

  adstring_array x_names(1,nvar);
  param.xinit(x, x_names);
  extra_names(param,x_names, nvar_model);
  cout << "xinit complete" << endl;
  for (int i = 1; i <= nvar; i++)
  {
    clogf << setw(5) << i << " - " << x_names(i) << endl;
  }

  hess_comp(param, nvar_model, x, recaps, nrec, 2, irfr);
  interflag.setsaveinter(0);  // free buffer

  dmatrix S(1,nvar_model,1,nvar_model);
  hess_inv(param, nvar_model, x, S, x_names);

  dvector scale(1,nvar_model);
  dvector stddev(1,nvar);
  sd_scale(scale, x, param);
  clogf << "\nScale:" << endl;
  for (int i = 1; i <= nvar_model; i++)
  {
    clogf << setw(3) << i << ": " << x_names(i)<< setw(12) << scale(i) << endl;
    cout  << setw(3) << i << ": " << x_names(i)<< setw(12) << scale(i) << endl;
  }

  obrien_derivs(S, x, scale, stddev, nvar_model, nvar_extra,
                param);
  model_sd(S, x, scale, stddev, nvar_model);

  clogf << "\nValue, StdDev"<< endl;
  for (int i = 1; i <= nvar; i++)
  {
    clogf << setw(4) << i << ": " << setw(10) << x_names(i) << ": "
         << setscientific << setw(16) << setprecision(8) << x(i)
         << setw(16) << sqrt(stddev(i)) << endl;
  }


  time(&time_sec);
  clogf << "finished time: " << ctime(&time_sec) << endl;
  cout  << "finished time: " << ctime(&time_sec) << endl;

  clogf.close();
  cout << "Done!" << endl;
  exit(0);
} /* end of main */

void model_sd(dmatrix& S, dvector& x, dvector& scale, dvector& std_dev,
              int nvar_model)
{
  for (int i = 1; i <= nvar_model; i++)
  {
    x(i) = scale(i);
    std_dev(i) = S(i,i)*scale(i)*scale(i);
  }
}
