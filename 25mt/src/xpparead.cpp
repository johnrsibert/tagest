//$Id: xpparead.cpp 2956 2012-03-11 01:34:21Z jsibert $
#include "par_t.h"
#include "bound.h"
#ifdef __NDPX__
  #include <iomanip.h>
#endif
#include "trace.h"
extern ofstream clogf;

adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
adstring& adstring_utils_toupper(adstring& ads);
adstring_array& adstring_utils_toupper(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

void set_ipar_names(ipar_t& t);

//template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
//par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t(const char* name, const adstring parversion)
//: Model_area(name,parversion),  m_ipar(100)
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::par_t(const ADMB_XMLDoc& doc)
: Model_area(doc),  m_ipar(100)
{
  
  cout << "Extracting XML par_t data from " << doc.getFileName() << endl;
  clogf << "Extracting XML par_t data from " << doc.getFileName() << endl;

  /*
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
  */

  //set_ipar_names(m_ipar);
  //read_ipar(dfile);
  m_ipar = doc.getIpar("m_ipar");
  for (int i = m_ipar.indexmin(); i <= m_ipar.indexmax(); i++)
    TTRACE(m_ipar(i),m_ipar.get_name(i))
  cout << "Finished reading m_ipars." << endl;
  clogf << "Finished reading m_ipars." << endl;

  int start_year=doc.getInt("start_yr");  // year during which first tags were released (-1900)
  int start_month=doc.getInt("start_mo"); // month of starting year first tags released

  //dfile >> nmonth >> start_year >> start_month >> nrelease >> nfleet;
  nmonth = doc.getInt("nmonth");
  nrelease = doc.getInt("nrelease");
  TRACE(nrelease)
  nfleet = doc.getInt("nfleet");
  TRACE(nfleet)
  clogf << "Finished reading nfleet." << endl;
  cout << __FILE__ << ':' << __LINE__ << endl;
  /*
  if ( !dfile )
  {
    cerr << "Error reading sizes in file " << fullname << endl;
    exit(1);
  }
  */

  num_afm = m_ipar(24);
  TRACE(num_afm)
  if (num_afm)
  {
     cerr << "fishing mortality anomailes (ipar[24]) no longer supported." << endl;
     ad_exit(1);
  }
  HERE
  allocate();
  HERE
  initialize();
  HERE
  start_date.set(start_year, start_month);
  TTRACE(start_year, start_month)

  //dfile >> mort;
  mort=doc.getDouble("mort");
  cout << "Finished reading mort = " << mort << endl;
  clogf << "Finished reading mort = " << mort << endl;

  /*
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
  */
  special_mort = doc.getDouble("special_mort");
  cout << "Finished reading special_mort = " << special_mort << endl;
  clogf << "Finished reading special_mort = " << special_mort << endl;

  //DomesticFleetKludgeFlag = dvector(doc.getDvector("DomesticFleetKludgeFlag"));
  DomesticFleetKludgeFlag = doc.getIvector("DomesticFleetKludgeFlag");
  TRACE(DomesticFleetKludgeFlag)

  //TRACE(doc.getADStringArray("fleet_names",1,nfleet))
  TTRACE(fleet_names.indexmin(),fleet_names.indexmax())

  //fleet_names = doc.getADStringArray("fleet_names",1,nfleet);
  doc.getADStringArray("fleet_names",&fleet_names);
  TRACE(fleet_names) 

  // read catchability coeff, by fleet, from .par file
    for (int f = 1; f <= nfleet; f++)
    {
      /*
      char buff[5];
      dfile >> buff >> q[f] >> report_rates[f];
    #ifndef OLD_DO_KLUDGE
      dfile >> DomesticFleetKludgeFlag(f);


    #else
      DomesticFleetKludgeFlag(f) = 0;
    #endif
      fleet_names[f] = adstring(buff);
      */
      adstring_utils_tolower(fleet_names[f]);
      cout << fleet_names[f] << "  " << q[f] << "  " << report_rates[f] 
           << "  " << DomesticFleetKludgeFlag(f) << endl;
      clogf << fleet_names[f] << "  " << q[f] << "  " << report_rates[f]
            << "  " << DomesticFleetKludgeFlag(f) << endl;

    }

  /*
  if (num_afm)
  {
    adstring afm_fleet;
    for (int i = 1; i <= num_afm; i++)
    {
      dfile >> afm_yr(i) >> afm_mo(i) >> afm_fleet >> afm_i(i) >> afm_j(i) >> afm(i);
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
  */

  HERE
  #define XML_TEST 
  #ifndef XML_TEST 
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
  if (m_ipar[50] == 0)
    del_x = deltax;
// cout << "del_x" << del_x << endl;
  #endif //XML_TEST 

}
template par_t<d3_array,dmatrix,dvector,double>::par_t(const ADMB_XMLDoc& doc);
template par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::par_t(const ADMB_XMLDoc& doc);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_ipar(cifstream& istr)
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
template void par_t<d3_array,dmatrix,dvector,double>::read_ipar(cifstream& istr);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,double>::read_ipar(cifstream& istr);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_tagrelease(cifstream& istr)
{
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

      cerr << "*   Tag release number " << k << ":" << endl;
      cerr << "    cohort code number " << cohort << endl;
      cerr << "    input longitude = " << longitude << " (" << tr[k].i << ")" << endl;
      cerr << "    input latitute  = " << latitude  << " (" << tr[k].j << ")" << endl;

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
      cerr << "**  Tag release number " << k << " is on domain boundary!" << endl;
      cerr << "    cohort code number " << cohort << endl;
      clogf << "**  Tag release number " << k << " is on domain boundary!" << endl;
      clogf << "    cohort code number " << cohort << endl;
      //exit(1);
    }
  /*
  #undef USE_EDGE_TEST
  #ifdef USE_EDGE_TEST
     bound boundary = cell_type[tr[k].i][tr[k].j];
    if ((boundary.edgex() == ISLAND) || (boundary.edgey() == ISLAND))
  #else
    if (gridmap[tr[k].i][tr[k].j] == 0)
  #endif
  */
   if (is_land(longitude,latitude))
    {
      cerr << "**  Tag release number " << k << " is on an island!" << endl;
      cerr << "    cohort code number " << cohort << endl;
      clogf << "**  Tag release number " << k << " is on an island!" << endl;
      clogf << "    cohort code number " << cohort << endl;
      TRACE(tr[k])
      TTRACE(longitude,latitude)
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
template void par_t<d3_array,dmatrix,dvector,double>::read_tagrelease(cifstream& istr);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::read_tagrelease(cifstream& istr);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::read_bounds(cifstream& dfile)
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
  HERE
  if (m_ipar[83] == 1)
  {
    if (m_ipar[84] == 1)
    {
      dfile >> minsus_spd >> maxsus_spd;
      cout << "minsus_spd maxsus_spd: " << minsus_spd << " " << maxsus_spd << endl;
    }
  
    if (m_ipar[85] == 1)
    {
      //dfile >> minMin_D >> maxMin_D;
      //cout << "minMin_D maxMin_D: " << minMin_D << " " << maxMin_D << endl;
      dfile >> minslope >> maxslope;
      cout << "minslope maxslope: " << minslope << " " << maxslope << endl;
      //dfile >> mininflection >> maxinflection;
      //cout << "mininflection maxinflection: " << mininflection << " " << maxinflection << endl;
    }
  }
  HERE
  
  if ( (m_ipar[11] == 4) || (m_ipar[11] == 5) ||
       (m_ipar[11] ==14) || (m_ipar[11] ==15) )
  {
    dfile >> nb_par(1) >> nb_par(2);
    clogf << "negative binomial parameter = " << nb_par << endl;
    cout << "negative binomial parameter = " << nb_par << endl;
  }

  if ( (m_ipar[11] ==24) || (m_ipar[11] ==25) )
  {
    dfile >> nb_par;
    clogf << "negative binomial parameter = " << nb_par << endl;
    cout << "negative binomial parameter = " << nb_par << endl;
  }

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
template void par_t<d3_array,dmatrix,dvector,double>::read_bounds(cifstream& dfile);
template void par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::read_bounds(cifstream& dfile);
