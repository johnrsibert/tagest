#include "precomp.h"
#include "fish_rec.h"
#pragma hdrstop
#include "trace.h"
#include <prnstrem.h>

#ifdef __TURBOC__
  #undef __ZTC__
  extern unsigned _stklen = 0x4000;
#endif

#ifdef __ZTC__	 
  unsigned _stack = 64000;
#endif
void readrecaps(par_t& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path = DEFAULT_PATH);

//imatrix displacement_bins(par_t& param, const double bin_width);
//void displacement_frequency(recaptype_vector& recaps, int nrecaps, par_t& param,
//                           dmatrix& displacements, 
//                           dmatrix& xdisp, dmatrix& ydisp, const imatrix& bins);
//void write_displacement_files(const dmatrix& displacement, 
//                         dmatrix& xdisp, dmatrix& ydisp, const double bin_width,
//                         const int fit_number, const char op);


dvector displacement_bins(par_t& param, dmatrix& distances);
void displacement_frequency(recaptype_vector& recaps, int nrecaps, par_t& param,
                            dmatrix& displacements, 
                            dmatrix& xdisp, dmatrix& ydisp, 
                            const dmatrix& distances, const dvector& bins);

void write_displacement_files(const dmatrix& displacement, 
                         dmatrix& xdisp, dmatrix& ydisp, 
                         const dvector& bins,
                         const int fit_number, const char op);

ofstream clogf("tac.log");
adstring make_banner(const adstring& program_name);
adstring banner;

void main(int argc, char ** argv)
{
  banner = make_banner(adstring(argv[0])+": observed tag attrition data generator");
  cout << banner << endl;
  clogf << banner << endl;
  HERE
  cifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    HERE
    cerr << "Cannot open file_name.tmp in main()" << endl;
    exit(1);
  }
  char filename[20];
  HERE
  fnt >> filename;
  HERE
  fnt.close();
  cout << "Read '" << filename << "' from file_nam.tmp" << endl;

#ifdef DOS386
  const int MAXRETURNS = 12000;     // increase for more tag returns
#else
  const int MAXRETURNS = 2500;     // less than 1 segment
#endif

  int  nrec = MAXRETURNS-1; /* used as check in readrecaps */
  recaptype_vector recaps(1, MAXRETURNS);
  TRACE(sizeof(recaptype))
  TRACE(sizeof(recaptype)*long(MAXRETURNS))
  TRACE(sizeof(recaptype)*MAXRETURNS)

//  par_t param(filename);

#if defined (__REGIONAL__)
  par_t_reg param(filename);
#elif defined (__HABITAT__)
  par_t_hab param(filename);
#else
  #error Error: Undefined par_t derived class
#endif
  //recaptype::set_cohort_pooling(param.m_ipar[13]);
  recaps.set_cohort_pooling(param.m_ipar[13]);

  indexed_regional_fishery_record irfr(filename);

  readrecaps(param, filename, recaps, nrec);
  TRACE(nrec)

  int ncolumns = 0;

  int pool_tags = param.m_ipar[13];
  if (pool_tags == 0)
    ncolumns = param.get_nrelease();
  else if (pool_tags == 1)
    ncolumns = param.get_nfleet();
  else if (pool_tags == 2)
  {
    param.collapse_cohorts(recaps, nrec);
    ncolumns = param.get_tr_cohort(param.get_nrelease());
  }
  TRACE(ncolumns)

  int max_month = param.get_tr_date(param.get_nrelease())
                - param.get_tr_date(1) + param.get_nmonth() + 1;

  if (max_month < 60)
    max_month = 60;                
  TRACE(max_month)
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
  dmatrix displacement(0,max_month+1,0,dbins.indexmax()); //70);
  dmatrix xdisp(0,max_month+1,-(m+1),m+1);
  dmatrix ydisp(0,max_month+1,-(n+1),n+1);

  dmatrix tac(0, max_month+1, 0, ncolumns);
  tac.initialize();
  ivector& jlb = param.get_jlb();
  ivector& jub = param.get_jub();
  dmatrix effort(1,m, jlb, jub);
  for (int k=1; k <= nrec; k++)
  {
    cerr << "\rProcessing recapture record" << setw(8) << k << "  ";
    int month = recaps[k].date - param.get_start_date();
    adstring fleet = recaps[k].fleet;
    fleet.to_lower();
    year_month date = recaps[k].date;
    int i = recaps[k].i;
    int j = recaps[k].j;
    int fl = param.fleet_names.index(fleet); // fl > 0 for valid fleet name

    if (month > max_month)
      month = max_month+1;
    int cohort_code = recaps[k].cohort;

    int effort_occured = irfr.get_effort(fleet,date,effort);
  //int effort_occured = irfr.get_effort(param.fleet_names(fl),date,effort);
    if ( (fl) && (effort_occured) && (effort(i,j) > 0) )
    {
      int cc = 0;
      int tal = 0;
      if (pool_tags==1)
      {
        cc = recaps[k].flindex;
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
  ss << "fit" << setw(2) << setfill('0') << hex << param.m_ipar[8]
     << "_P" << pool_tags << ".ota" << '\0';
  prnstream prn(prn_name);

  prn << "observed tag attrition curve from .tag file" << PENDL;
  prn << "months at" << "aggregate";
  if (pool_tags == 0)
    prn << "release group" << PENDL;
  else if (pool_tags == 1)
    prn << "recapture fleet" << PENDL;
  else if (pool_tags == 2)
    prn << "monthly aggregated release group" << PENDL;

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
  prn << PENDL;

  for (int tal=0; tal <= max_month+1; tal++)
  {
    prn << tal << r[tal];
    //cout << tal << "  " << r[tal] << " | ";// << endl;
    for (int column = 0; column <= ncolumns; column++)
    {
      if ( tac(tal,column) <=0)
         tac(tal,column) = 1e-8;
      prn << tac(tal,column);
      //cout << "  " << tac(tal,column);
    }
    prn << PENDL;
    //cout << endl;
  }
  prn.close();

  if (pool_tags == 0)
  {
    displacement_frequency(recaps, nrec, param, displacement, xdisp, ydisp, 
                           distances, dbins);
    write_displacement_files(displacement, xdisp, ydisp, dbins, param.m_ipar[8],'o');
  }
  exit(0);
} // end of main

