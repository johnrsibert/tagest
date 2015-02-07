#include "par_t.h"
#include "bound.h"
#ifdef __NDPX__
  #include <iomanip.h>
#endif
#include "trace.h"
extern ofstream clogf;
 
adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

void set_ipar_names(ipar_t& t);

par_t::par_t(const char* name)
: Model_area(name),  m_ipar(150)
{
  HERE
  adstring fullname;

  cout << "Reading part_t data for " << name << endl;
  clogf << "Reading part_t data for " << name << endl;

  fullname = adstring(name);
  int dot_pos = fullname.pos(".");
  if (dot_pos == 0)
  {
    fullname += ".par";
  }
 
  cifstream dfile(fullname);
  if ( !dfile )
  {
    cerr << "Cannot reopen file " << fullname << endl;
    exit(1);
  }

  //start where model_area finished
  dfile.seekg(sp);
  set_ipar_names(m_ipar);
  // read m_ipars from .par file
  read_ipar(dfile);
  cout << "Finished reading m_ipars." << endl;
  clogf << "Finished reading m_ipars." << endl;


  int start_year;  // year during which first tags were released (-1900)
  int start_month; // month of starting year first tags released

  dfile >> nmonth >> start_year >> start_month >> nrelease >> nfleet;
  clogf << "Finished reading nfleet." << endl;

  if ( !dfile )
  {
    cerr << "Error reading sizes in file " << fullname << endl;
    exit(1);
  }

  num_afm = m_ipar(24);
  allocate();
  initialize();
  start_date.set(start_year, start_month);

  dfile >> mort;
  if (m_ipar[19] > 0)
  {
    dfile >> special_mort;
    cout << "Finished reading special mortality " << special_mort << endl;
    clogf << "Finished reading special mortality " << special_mort << endl;
  }
  else
  {
    special_mort = 0.0;
  }
//  cout << "Finished reading mort = " << mort << endl;
  clogf << "Finished reading mort = " << mort << endl;

  // read catchability coeff, by fleet, from .par file
    for (int f = 1; f <= nfleet; f++)
    {
      char buff[5];
      dfile >> buff >> q[f] >> report_rates[f];
      fleet_names[f] = adstring(buff);
      //fleet_names[f].to_lower();
      adstring_utils_tolower(fleet_names[f]);
      cout << fleet_names[f] << "  " << q[f] << "  " << report_rates[f] << endl;
      clogf << fleet_names[f] << "  " << q[f] << "  " << report_rates[f] << endl
;
    }
 
  if (num_afm)
  {
    adstring afm_fleet;
    for (int i = 1; i <= num_afm; i++)
    {
      dfile >> afm_yr(i) >> afm_mo(i) >> afm_fleet >> afm_i(i) >> afm_j(i) >> afm(i);
      //afm_fl(i) = fleet_names.index(afm_fleet);
      afm_fl(i) = adstring_utils_index(fleet_names, afm_fleet);

       clogf << "afm " << i << "  " << afm_yr(i) << "  " << afm_mo(i)
             << "  " <<afm_fl(i) << "  " << afm_i(i) << "  " << afm_j(i)
             << "  " << afm(i) << endl;
       cout  << "afm " << i << "  " << afm_yr(i) << "  " << afm_mo(i)
             << "  " <<afm_fl(i) << "  " << afm_i(i) << "  " << afm_j(i)
             << "  " << afm(i) << endl;
    }
    TRACE(afm)
  }
  
  read_tagrelease(dfile);
  cout << "Finished reading tag releases." << endl;

  read_bounds(dfile);
  cout << "Finished reading parameter bounds." << endl;

  // return current position in file
  sp = dfile.tellg();

  clogf << "Fishished reading generic parameters." << endl;
  cout << "Fishished reading generic parameters." << endl;
  dfile.close();

  // Check to see if to set deltax vector to 60//
  if (m_ipar[85] == 0)
    del_x = deltax;
// cout << "del_x" << del_x << endl;
}

void par_t::read_ipar(cifstream& istr)
{
  cout << m_ipar[0] << " m_ipars:" << endl;
  for (int i = 1; i <= m_ipar[0]; i++)
  {
    istr >> m_ipar(i);
    if (!istr)
    {
      cerr << "Error reading m_ipar[" << i << "]" << endl;
      exit(1);
    }
  }
}

void par_t::read_tagrelease(cifstream& istr)
{
  bound boundary;
  /*union
  {
    unsigned short int boundary;
    struct
    {
      char x;
      char y;
    } edge;
  };*/

  double longitude;
  double latitude;
  int cohort, year, month;
  for (int k=1; k <= nrelease; k++)
  {
    istr >> cohort;
    tr[k].cohort = cohort;
    tr[k].release_cohort = cohort;
    istr >> year >> month;
    tr[k].date.set(year,month);
    istr >> longitude;
    tr[k].i = long_to_index(longitude); //move_corner(longitude, i_sw_long);
    istr >> latitude;
    tr[k].j = lat_to_index(latitude); //move_corner(latitude, i_sw_lat);
    istr >> tr[k].tags;
    if (!istr)
    {
       cerr << "**  error reading tag release number " << k << endl;
       exit(1);
    }

    if ((tr[k].i < 1) || (tr[k].i > m) ||
	(tr[k].j < 1) || (tr[k].j > n))
    {
      cerr << "**  Tag release number " << k << " out of range!" << endl;
      cerr << "    cohort code number " << cohort << endl;
      cerr << "    input longitude = " << longitude << " (" << tr[k].i << ")" << endl;
      cerr << "    input latitute  = " << latitude  << " (" << tr[k].j << ")" << endl;
      exit(1);
    }

    if ((tr[k].i == 1) || (tr[k].i == m) ||
	(tr[k].j == 1) || (tr[k].j == n))
    {
      cerr << "**  Tag release number " << k << " is on an edge!" << endl;
      cerr << "    cohort code number " << cohort << endl;
      exit(1);
    }

    boundary = cell_type[tr[k].i][tr[k].j];
    if ((boundary.edgex() == ISLAND) || (boundary.edgey() == ISLAND))
    {
      cerr << "**  Tag release number " << k << " is on an island!" << endl;
      cerr << "    cohort code number " << cohort << endl;
      exit(1);
    }

    if (k>1)
    {
      if (tr[k-1].date > tr[k].date)
      {
	 cerr << "**  Tag release number " << k
	      << " is earlier than tag release number " << (k-1) << endl;
	 exit(1);
      }
    }

  }
}

void par_t::read_bounds(cifstream& dfile)
{
  dfile >> minsig >> maxsig;
  clogf << "minsig, maxsig:\n" << minsig << " " << maxsig << endl;

  dfile >> minmort >> maxmort;
  clogf << "minmort, maxmort:\n" << minmort << " " << maxmort << endl;

  dfile >> minq >> maxq;
  clogf << "minq, maxq:\n" << minq << " " << maxq << endl;

  // read reporting rates max and min, from .par file, since valid version
  dfile >> minr_rates >> maxr_rates;
  clogf << "minr_rates, maxr_rates:\n"
        << minr_rates << "  " << maxr_rates << endl;

  dfile >> minvel >> maxvel;
  clogf << "minvel, maxvel:\n" << minvel << "  " << maxvel << endl;

  /*
  dfile >> mindmax >> maxdmax;
  clogf << mindmax << "  " << maxdmax << endl;
  dfile >> mindslope >> maxdslope;
  clogf << mindslope << "  " << maxdslope << endl;
  dfile >> minchi >> maxchi;
  clogf << minchi << "  " << maxchi << endl;
  dfile >> mintheta >> maxtheta;
  clogf << mintheta << "  " << maxtheta << endl;
  dfile >> minatemp >> maxatemp;
  clogf << minatemp << "  " << maxatemp << endl;
  */

  fpen = 0;
  if (!dfile)
  {
    cerr << "**  error reading parameter bounds" << endl;
    exit(1);
  }
}

