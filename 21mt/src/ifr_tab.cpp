#include <fvar.hpp>
#include "fish_rec.h"
#include <iomanip>

void indexed_regional_fishery_record::tabulate(void)
{
  tabulate(cout);
}

void indexed_regional_fishery_record::tabulate(ostream& sout)
{
  imatrix incidents(first_year, last_year, 1, 12);
  dmatrix total_effort(first_year, last_year, 1, 12);

  for (int fl = 1; fl <= nfleet; fl++)
  {
    incidents.initialize();
    total_effort.initialize();
    for (int yr = first_year; yr <= last_year; yr++)
    {
      for (int mo = 1; mo <= 12; mo++)
      {
        int n1 = index(fl,yr,mo);
        int n2 = n1+nrec(fl,yr,mo)-1;
        for (int n = n1; n <= n2; n++)
        {
          incidents(yr,mo) ++;
          total_effort(yr,mo) += fishery_records[n].effort;
        } // n loop
      } // mo loop
    } // yr loop

    sout << "\nIFR fishing incident summary for fleet " << fleet_list(fl) << endl;
    for (int yr = first_year; yr <= last_year; yr++)
    {
      sout << setw(5) << yr;
      for (int mo = 1; mo <= 12; mo++)
      {
        sout << setw(5) << incidents(yr,mo);
      }
      sout << endl;
    }

    sout << "\nIFR fishing total effort summary for fleet " << fleet_list(fl) << endl;
    for (int yr = first_year; yr <= last_year; yr++)
    {
       sout << setw(5) << yr;
       for (int mo = 1; mo <= 12; mo++)
      {
        sout << "  " << total_effort(yr,mo);
      }
      sout << endl;
    }


  } // fl loop
} // end of tabulate

#ifdef __IFR_TAB_TEST__
ofstream clogf;
int main(int argc, char ** argv)
{
  if (argc <= 1)
  {
    cerr << "Enter ifr file root name on command line." << endl;
    exit(1);
  }

  clogf.open("ifr_tab.log");
  adstring file_name(argv[1]);
  //file_name += ".ifr";
  cout << "Tabulating IFR file for " << file_name << endl;

  indexed_regional_fishery_record irfr(file_name);

  irfr.tabulate();
  irfr.tabulate(clogf);

  exit(0);
} // end of main
#endif //#ifdef __IFR_TAB_TEST__



