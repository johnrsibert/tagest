//$Id: tac.cpp 3098 2013-08-15 18:16:04Z jsibert $
/** 
\file tac.cpp. Code to generate .csv files for creating tag attirtion 
and related plots. Does NOT distinguish between recaptures with effort and
recaptures without effort.
\deprecated Superceded by ttac.cpp
*/
#include "intersav.h"
#include "recap_t.h"
#include "fish_rec.h"
#include "trace.h"
#include <prnstrem.h>
#include <strstream>
using std::ostrstream;

#ifdef __TURBOC__
  #undef __ZTC__
  extern unsigned _stklen = 0x4000;
#endif

#ifdef __ZTC__	 
  unsigned _stack = 64000;
#endif

#ifdef __APPLE__
template class par_t<d3_array,dmatrix,dvector,double>;
template class par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
#endif

dvector displacement_bins(par_t<d3_array,dmatrix,dvector,double>& param, dmatrix& distances);
void displacement_frequency(recaptype_vector& recaps, int nrecaps, par_t<d3_array,dmatrix,dvector,double>& param, dmatrix& displacements, dmatrix& xdisp, dmatrix& ydisp, const dmatrix& distances, const dvector& bins); 

void write_displacement_files(const dmatrix& displacement, 
                         dmatrix& xdisp, dmatrix& ydisp, 
                         const dvector& bins,
                         const int fit_number, const char op);

int _global_report_flag = 0;
int saveinterflag = 1;
setsaveinterflag interflag;
ofstream clogf("tac.log");
adstring make_banner(const adstring& program_name);
adstring banner;
extern "C"  {
  void ad_boundf(int i)
  {
    // so we can stop here
    exit(i);
  }
} 


int main(int argc, char ** argv)
{
  ad_exit=&ad_boundf;

  banner = make_banner(adstring(argv[0])+": observed tag attrition data generator");
  cout << banner << endl;
  clogf << banner << endl;
  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_name.tmp in main()" << endl;
    exit(1);
  }
  char filename[20];
  fnt >> filename;
  fnt.close();
  cout << "Read '" << filename << "' from file_nam.tmp" << endl;


  par_t<d3_array,dmatrix,dvector,double>::createInstance(filename);
  if (par_t<d3_array,dmatrix,dvector,double>::instance == 0)
  {
    return 0;
  }
  par_t<d3_array,dmatrix,dvector,double> &param = *par_t<d3_array,dmatrix,dvector,double>::instance;


  indexed_regional_fishery_record irfr(filename);

  recaptype_vector *precaps = recaptype_vector::createInstance(filename, param, irfr);
  const int nrec = precaps->getTotalRecords();
  recaptype_vector& recaps = *precaps;
        recaps.set_cohort_pooling(param.m_ipar[13]);

  int ncolumns = 0;

  int pool_tags = param.m_ipar[13];
  TRACE(pool_tags)
  if (pool_tags == 0)
    ncolumns = param.get_nrelease();
  else if (pool_tags == 1)
    ncolumns = param.get_nfleet();
  else if (pool_tags == 2)
  {
    param.collapse_cohorts(recaps, nrec);
    ncolumns = param.get_tr_cohort(param.get_nrelease());
  }

  int max_month = param.get_tr_date(param.get_nrelease())
                - param.get_tr_date(1) + param.get_nmonth() + 1;

  if (max_month < 60)
    max_month = 60;                
  dvector r(0,max_month+1);
  r.initialize();

  int m = param.get_m();
  int n = param.get_n();
  double bin_width = 60.0;
  if (param.m_ipar[46] > 0)
    bin_width = param.m_ipar[46];
  //imatrix dbins = displacement_bins(param, bin_width);
  dmatrix distances(0,m,0,n);
  dvector dbins = displacement_bins(param, distances);
  dmatrix displacement(0,max_month+1,0,dbins.indexmax()+1); //70);
  TTRACE((max_month+1),dbins.indexmax())
  dmatrix xdisp(0,max_month+1,-(m+1),m+1);
  dmatrix ydisp(0,max_month+1,-(n+1),n+1);

  dmatrix tac(0, max_month+1, 0, ncolumns);
  tac.initialize();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();
  dmatrix effort(1,m, jlb, jub);
  for (int k=1; k <= nrec; k++)
  {
    adstring& adstring_utils_tolower(adstring& ads);

    cerr << "\rProcessing recapture record" << setw(8) << k << "  ";
    int month = recaps[k].date - param.get_start_date();
    adstring fleet = recaps[k].fleet;
    adstring_utils_tolower(fleet);
    year_month date = recaps[k].date;
    int i = recaps[k].i;
    int j = recaps[k].j;
    int adstring_utils_index(adstring_array& ads_array, const adstring& ads);
    int fl = adstring_utils_index(param.fleet_names, fleet); // fl > 0 for valid fleet name

    if (month > max_month)
      month = max_month+1;
    int cohort_code = recaps[k].cohort;

    int effort_occured = irfr.get_effort(fleet,date,effort);
    //if ( (fl) && (effort_occured) && (effort(i,j) > 0) )
    if (fl)
    {
      int cc = 0;
      int tal = 0;
      if (pool_tags==1)
      {
        cc = recaps[k].par_flindex;
        tal = recaps[k].date - param.get_start_date();
      }
      else
      {
        int cohort = 0;
        for (cohort = 1; cohort <= param.get_nrelease(); cohort++)
        {
          if (cohort_code == param.get_tr_cohort(cohort) )
          {
            break;
          }
        }
        tal = recaps[k].date - param.get_tr_date(cohort);
        if (tal < 0)
        {
          clogf << "!! recapture " << k << " on " <<recaps[k].date
                << " seems to have occured before the release of cohort "
                << cohort << " on " <<  param.get_tr_date(cohort) << endl;
          clogf << "!! tal set to zero" << endl;
          tal = 0;
        }
        if (pool_tags == 0)
          cc = cohort;
        else  // 2
          cc = cohort_code;

      }

      if (tal > max_month)
        tal = max_month+1;
      r[month] += recaps[k].returns;
      double pcaps = recaps[k].returns;
      tac(tal, cc) += pcaps;
      tac(tal,0) += pcaps;
    }
  }

  char prn_name[80];
  ostrstream ss(prn_name,80);
  ss << "fit" << setw(3) << setfill('0') << hex << param.m_ipar[8]
     << "_P" << pool_tags << "_ota.csv" << '\0';
  prnstream prn(prn_name);

  prn << "observed tag attrition curve from .tag file" << endl;
  prn << "months at" << "aggregate";
  if (pool_tags == 0)
    prn << "release group" << endl;
  else if (pool_tags == 1)
    prn << "recapture fleet" << endl;
  else if (pool_tags == 2)
    prn << "monthly aggregated release group" << endl;

  prn << "liberty" << "recaptures";
  for (int column = 0; column <= ncolumns; column++)
  {
    if (column == 0)
      prn << "All";
    else
    {
      if (pool_tags == 0)
        prn << param.get_tr_cohort(column);

      else if (pool_tags == 1)
        prn << (char*)param.fleet_names(column);

      else if (pool_tags == 2)
        prn << column;

    }
  }
  prn << endl;

  for (int tal=0; tal <= max_month+1; tal++)
  {
    prn << tal << r[tal];
    //cout << tal << "  " << r[tal] << " | ";// << endl;
    for (int column = 0; column <= ncolumns; column++)
    {
      if ( tac(tal,column) < 0)
         tac(tal,column) = 1e-8;
      prn << tac(tal,column);
      //cout << "  " << tac(tal,column);
    }
    prn << endl;
    //cout << endl;
  }
  prn.close();
  if (pool_tags == 0)
  {
    displacement_frequency(recaps, nrec, param, displacement, xdisp, ydisp, 
                           distances, dbins);
    write_displacement_files(displacement, xdisp, ydisp, dbins, param.m_ipar[8],'o');
  }
  return 0;
}
