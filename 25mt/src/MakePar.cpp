//$Id: MakePar.cpp 2848 2011-08-25 17:54:33Z jsibert $
/** \file MakePar.cpp
Utility to create initial ".par" file.
Sets and initializes some global variables.
\ingroup preproc
*/

#include <fvar.hpp>
#include <adstring.hpp>
#include "intersav.h"
#include "par_t_reg.h"
#include "recap_t.h"


ofstream clogf;
adstring make_banner(const adstring& program_name);
void rpreproc_main(char c, const adstring& path);
adstring banner;
int _global_report_flag = 0;
int saveinterflag = 1;
setsaveinterflag interflag;
template <typename D3_ARRAY,typename MATRIX,typename VECTOR,typename DOUBLE>
void readrecaps(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, const char* root, recaptype_vector& recaps,
                int& pnrecs, const char* path, indexed_regional_fishery_record& irfr);


extern "C"  {
  void ad_boundf(int i)
  {
    // so we can stop here
    exit(i);
  }
} 

/**
main() for the ".par" file generation application.
Sets exit procudure and calls  rpreproc_main('p', "").
\ingroup preproc
*/
int main(int argc, char ** argv)
{
  ad_exit=&ad_boundf;
  clogf.open("makepar.log");
  banner = make_banner(adstring(argv[0])+": initial par file generation utility\n");  
  cout << banner << endl;
  clogf << banner << endl;
  rpreproc_main('p', "");

  HERE
  cout << "\nStarting sanity check:" << endl;
  clogf << "\nStarting sanity check:" << endl;
  ifstream fnt("file_nam.dat");
  if ( !fnt)
  {
    cerr << "Error: Cannot open file_nam.dat in MakePar.cpp)";
    exit(1);
  }
  adstring root_filename(1, 9);
  adstring tag_data_filename(1, 9);
  adstring date_filename(1, 9);
  adstring fleet_filename(1, 9);
  fnt >> root_filename >> tag_data_filename >> date_filename >> fleet_filename;
  clogf << "Read '" << root_filename << "' from file_nam.dat" << endl;
  clogf << "Read '" << tag_data_filename << "' from file_nam.dat" << endl;
  clogf << "Read '" << date_filename << "' from file_nam.dat" << endl;
  clogf << "Read '" << fleet_filename << "' from file_nam.dat" << endl;
  par_t_reg<d3_array,dmatrix,dvector,double> param(root_filename);
  indexed_regional_fishery_record ifr;
  ifr.read(root_filename,"./");

  const int MAXRETURNS = 82000;     // increase for more tag returns
  int  nrec = MAXRETURNS-1;
  recaptype_vector recaps(1, MAXRETURNS);
  readrecaps(param, root_filename, recaps, nrec, "", ifr);

  double tObsCaps = 0.0;
  const int ngrid = param.ngrid;
  dvector ObsRegionCaps(0,ngrid);
  ObsRegionCaps.initialize();
  for (int k = 1; k <= nrec; k++)
  {
    const int i = recaps[k].i;
    const int j = recaps[k].j;
    const int r = param.gridmap(i,j);
    ObsRegionCaps(r) += recaps[k].returns;
    tObsCaps += recaps[k].returns;
  }
  clogf << "\nTag recaptures by region:" << endl;
  for (int k = 1; k <= ngrid; k++)
  {
    clogf << setw(3) << k 
          << setw(8) << setprecision(0) << ObsRegionCaps[k] << endl;
  }
  clogf << "Sum" << setw(8) << setprecision(0) << tObsCaps << endl;


  ivector effortless = recaps.count_effortless_recaps();
  clogf << "\nRecaptures WITHOUT fishing effort: " << effortless(0) << endl;
  clogf << "Recaptures  with   fishing effort: " << effortless(1) << endl;

  recaps.tabulate_effortless_recaps(clogf,param.fleet_names);


  return 0;
}
