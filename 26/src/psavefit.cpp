//$Id: psavefit.cpp 3129 2014-02-03 22:10:56Z eunjung $
#include "par_t.h"
ostream& setfixed(ostream& _s);


//const extern  double min_nb_par;
//const extern  double max_nb_par;
extern ofstream clogf;

 
void par_t::savefit(ofstream& pf, const char* fullname)
{
  if (!pf)
  {
    cerr << "Cannot open file " << fullname << endl;
    exit(1);
  }
  pf << par_file_version
     << "\n# " << fullname
     //<< "\n# " << executable
     << "\n#\n#   M    N  deltax  deltay  sw_long sw_lat\n"
     << setw(5) << m << setw(5) << n
     << setw(7) << deltax << setw(7) << deltay << "   " << sw_coord << endl;

  pf << "# grid map\n";
  pf << "#   1";
  for (int i=2; i<=m; i++)
     pf << setw(5) << i;
  pf << "\n#\n";

  for (int j=n; j>=1; j--)
  {
     for (int i=1; i<=m; i++)
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
  for (int k = 1; k <= nrelease; k++)
  {
     if (tr[k].cohort > 0)
       posrel++;
  }

  pf << "\n#   nmonth start_yr start_mo  nrelease \n"
     << setw(8) << nmonth
     << setw(12) << start_date.get_year()
     << setw(12) << start_date.get_month_value()
     << setw(12) << posrel  //Do not count zero cohorts
     << "\n#   nfleet \n"
     << setw(8) << nfleet
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


  pf << "#\n# fleet             catchability           reporting rate  do fleet kludge\n";
  for (int i=1; i <= nfleet; i++)
  {
    pf << setw(8) << (char*)fleet_names[i] << " "
       << setw(25) << setprecision(15) << q(i)
       << setw(25) << setprecision(15) << report_rates[i]
       << setw(5) << DomesticFleetKludgeFlag(i)
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
  for (int k = 1; k <= nrelease; k++)
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
     << "    " << minsig << "   " << maxsig << endl
     << "# minmort   maxmort\n"
     << "   "  <<  minmort << "    " << maxmort << endl
     << "# minq      maxq\n"
     << "   "  <<  minq << "    " <<  maxq << endl
     << "# minr_rates   maxr_rates\n"
     << "   "  <<  minr_rates << "      " <<  maxr_rates << endl
     << "# minvel   maxvel\n"
     << "   "  <<  minvel << "   "  << maxvel << endl;
  if(m_ipar(83) ==1)
  {
     if(m_ipar(84) ==1)
     {
         //pf<< "# minsus_spd      maxsus_spd\n"
         //<< "   "  <<  minsus_spd << "    " <<  maxsus_spd << endl;
         pf<< "# minchi      maxchi\n"
         << "   "  <<  minchi << "    " <<  maxchi << endl;
     }

     if(m_ipar(85) == 1)
     {
       pf << "# minmin_D   maxmin_D\n"
          << "   " << minmin_D << "        " << maxmin_D << endl;
       pf << "# minlogslope maxlogslope\n"
          << "   " << minlogslope << "   " << maxlogslope << endl;
          //<< "# mininflection maxinflection\n"
          //<< "   " << mininflection  << "        " << maxinflection << endl;
     }
     else if(m_ipar(85) == 2)
     {
       pf << "# minloggamma   maxloggamma\n"
          << "   " << minloggamma << "        " << maxloggamma << endl;
     }
  }  
  /*
  if ( (m_ipar[11] == 4) || (m_ipar[11] == 5) ||
       (m_ipar[11] ==14) || (m_ipar[11] ==15) ||
       (m_ipar[11] ==24) || (m_ipar[11] ==25) )
  {
    pf << "#" << endl;
    pf << "# min_nb_par = " << min_nb_par << endl;
    pf << "# max_nb_par = " << max_nb_par << endl;
    pf << "# negative binomial parameters:" << endl;
    if ( (m_ipar[11] ==24) || (m_ipar[11] ==25) )
    {
       for (int f = 1; f <= nfleet; f++)
       {
          pf << "# " << (char*)fleet_names[f] << endl;
          pf  << setw(23) << setprecision(16)  << nb_par(f) << endl;
       }
    }
    else
      pf  << setw(23) << setprecision(16)  << nb_par << endl;
  }
  */
}
