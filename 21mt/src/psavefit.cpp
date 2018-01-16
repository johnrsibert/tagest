#include "par_t.h"
#include <fstream>
#include "trace.h"
ostream& setfixed(ostream& _s);

extern ofstream clogf;

void par_t::savefit(ofstream& pf, const char* fullname)
{
  int     i, i1, i2, j, jj, k, s;


  if (!pf)
  {
    cerr << "Cannot open file " << fullname << endl;
    exit(1);
  }
  pf << par_file_version 
     << "\n# " << fullname
     << "\n#\n#   M    N  deltax  deltay  sw_long sw_lat\n"
     << setw(5) << m << setw(5) << n
     << setw(7) << deltax << setw(7) << deltay << "   " << sw_coord << endl;

  pf << "# grid map\n";
  pf << "#   1";
  for (i=2; i<=m; i++) 
     pf << setw(5) << i;
  pf << "\n#\n";

  for (j=n; j>=1; j--)
  {
     for (i=1; i<=m; i++)
     {
       pf << setw(5) << gridmap(i, j);
     }
     pf << endl;
  }
  pf << "#\n#  boundary conditions:\n"
     << "#  west_bndry    east_bndry    south_bndry    north_bndry\n"
     << setw(8) << west_bndry
     << setw(14) << east_bndry
     << setw(14) << south_bndry
     << setw(14) << north_bndry
     << endl;

  pf << m_ipar;

  //Count releases > 0
  int posrel = 0;   
  for (k = 1; k <= nrelease; k++)
  {
     if (tr[k].cohort > 0)
       posrel++;
  }
 
  pf << "#\n#   nmonth start_yr start_mo  nrelease \n"
     << setw(8) << nmonth
     << setw(9) << start_date.get_year()
     << setw(9) << start_date.get_month_value()
     << setw(9) << posrel  //Do not count zero cohorts
     << "\n#   nfleet \n"
     << setw(9) << nfleet
     << "\n#" << endl;

  pf << "#  Model Parameters:\n"
     << "#          natural\n"
     << "#          mortality\n"
     << setw(25) <<  setprecision(17) << mort
     << "\n#" << endl;

  if (m_ipar[19] > 0)
  {
     pf << "# special mortality\n";
     pf << setw(25) << setprecision(17) << special_mort 
        << "\n#" << endl;
  }


  pf << "#\n# fleet             catchability           reporting rate\n";
  for (i=1; i <= nfleet; i++)
  {
    //4 SHOULD BE A MACRO TALK TO JOHN LATERS
    //fleet_names[i][4] = '\0';
    pf << setw(8) << (char*)fleet_names[i] << " "
       << setw(25) << setprecision(15) << q(i)
       << setw(25) << setprecision(15) << report_rates[i]
       << endl;
  }


   if (num_afm)
  {
    pf << "#\n# " << num_afm << " fishing mortality anomalies" << endl;
    //year_month afm_date;
    adstring afm_fleet;
    for (int i = 1; i <= num_afm; i++)
    {
       //afm_date.set(afm_yr(i),afm_mo(i));
       afm_fleet = fleet_names(afm_fl(i));
       pf << afm_yr(i) << "  " << afm_mo(i) << "  " << afm_fleet << "  " << afm_i(i)
          << "  " << "  " <<  afm_j(i)
          << setw(25) << setprecision(15) << afm(i) << endl;

    }
  }


  pf << "#\n#  tag releases:\n"
     << "# cohort    year   month     i     j number\n";
  for (k = 1; k <= nrelease; k++)
  {
     if (tr[k].cohort > 0)
     {
       pf << setfixed
//        << setw(8) << get_tr_cohort(k) //  cohort_index(k)
          << setw(8) << get_tr_release_cohort(k)
          << setw(8) << get_tr_date(k).get_year()
          << setw(8) << get_tr_date(k).get_month_value()
          << setw(8) << setprecision(2) << index_to_long(get_tr_i(k))
          << setw(8) << setprecision(2) << index_to_lat(get_tr_j(k))
          << setw(7) << int(get_tr_tags(k))
          << endl;
        clogf << setfixed << setprecision(5);  
     }
  }
  pf << "#\n# parameter bounds\n"
     << "# minsig   maxsig\n"
     << setprecision(8)
     << "    " << minsig << "  " << maxsig << endl
     << "# minmort   maxmort\n"
     << "    "  <<  minmort << "  " << maxmort << endl
     << "#    minq      maxq\n"
     << "    "  <<  minq << "  " <<  maxq << endl
     << "#    minr_rates      maxr_rates\n"
     << "    "  <<  minr_rates << "  " <<  maxr_rates << endl
     << "# minvel   maxvel\n"
     << "    "  <<  minvel << "  "  << maxvel << endl;
}
