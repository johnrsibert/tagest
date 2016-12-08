//$Id: halfproc.cpp 2955 2012-02-21 01:15:21Z jsibert $
/** \file halfproc.cpp
Code for halflife computation
*/

#include <prnstrem.h>
#include "par_t_reg.h"
#include <jnigraphics2.h>
#include "trace.h"

#include <sstream>
#include <ostream>
//using std::ostringstream;

extern int saveinterflag;
extern indexed_regional_fishery_record global_irfr;


#ifdef __BCG_SOLVER__
  #include "linbcg.h"
#else
  #include "coff_t.h"
  #include "intersav.h"
  extern setsaveinterflag interflag;
  extern intersavetype *isp;
#endif

void assignSeapodym(dmatrix& density, const dmatrix& seapodym);
void startingDensity(dmatrix& density, const dmatrix& initial);
void initialDensity(const dmatrix& density, const imatrix map, dvector& initial_density);
void densitycomp(const dmatrix& density, const imatrix map,
                 const double curr_time, dmatrix& zone_density);
void remove_tags(dmatrix& density, const imatrix map, const int region_drop);
void initial_prev_zs(const dvector& sum0, dvector& prev_sum);
void halfcomp(const dmatrix& density, const imatrix map, dvector& sum0, 
              dvector& prev_sum, dvector& cur_sum, 
              const double cur_time, double& prev_time,
              const double half, dvector& half_life);
void update_average(const dvector& current, dvector& average, 
                    const double& curr_time, double& prev_time);
void halfcomp(const dmatrix& density, dmatrix& prev_density, 
              const double curr_time, double& prev_time,
              const double half, const dmatrix& ini_density,
              dmatrix& half_life);
void update_average(const dmatrix& current, dmatrix& average, 
                    const double& curr_time, double& prev_time);
d3_array make_d3_array(int sl,int sh, int nrl, int nrh,
                       const ivector& ncl, const ivector& nch);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void get_effort_array(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, indexed_regional_fishery_record& irfr,
                      d3_array& effort, year_month& date, 
                      ivector& effort_occured);


extern ofstream clogf;
// copied from hesscmp2.cpp
int file_exists(const char* name)
{
  ifstream f(name);
  if (f)
    return(1);
  else
    return(0);
}

template <>
void par_t_reg<d3_array,dmatrix,dvector,double>::densitysim(indexed_regional_fishery_record& irfr)
{
  cout << __FILE__ << ": par_t_reg::densitysim" << endl;
  irfr.tabulate(clogf);
  int m_ipar68 = m_ipar[68];  // graphics off

  int graphics_on = m_ipar68;
  TTRACE(m_ipar68,graphics_on)

  ivector jlb1(0, m + 1);
  ivector jub1(0, m + 1);
  for (int i = 1; i <= m; i++)
  {
   jlb1(i) = jlb(i) - 1;
   jub1(i) = jub(i) + 1;
  }//fori
  jlb1(0) = 0;
  jlb1(m + 1) = 0;
  jub1(0) = n + 1;
  jub1(m + 1) = n + 1;


#ifdef unix
  adstring coastline_file = "../"+pathname.getPathRoot()+".cst";
#else
  adstring coastline_file = "..\\"+pathname.getPathRoot()+".cst";
#endif
  TRACE(coastline_file)

  int max_month = tr[nrelease].date - tr[1].date + nmonth + 1;
  if (max_month < 60)
    max_month = 60;
  TRACE(max_month)

  jnigraphics2* jni = NULL;
  TRACE(jni)

  const int ntabs = 3;
  ivector tab_list(1,ntabs);
  ivector mt_layout;
  ivector td_layout;
  int monthly_tab = 0;
  int average_tab = 0;
  int density_tab = 0;

  imatrix zonemap(1,m,1,n);
  adstring zm_name = "../"+pathname.getPathRoot()+".eez";
  cifstream zf(zm_name);
  if (!zf)
  {
    cerr << "Unable to open eez map file, " << zm_name << endl;
    exit(1);
  }
  
  int nzone = 100;
  //int nzone = max(zonemap);
  dvector zonesum0(0,nzone);
  zonesum0.initialize();
  for (int j=n; j>=1; j--)
  {
    for (int i=1; i<=m; i++)
    {
      zf >> zonemap(i,j);
      if (!zf)
      {
        cerr << "Error reading zonemap" << endl;
        exit(1);
      }
      int k = zonemap(i,j);
      zonesum0(k) ++;
    }
  }
  {
  clogf << "\nNumber of cells per eez" << endl;
  for (int k = 0; k <= nzone; k++)
  {
    clogf << setw(5) << k << setw(12) << zonesum0(k) << endl;
  }
  }

  dmatrix seedtags(1, m, jlb, jub); // Inital distribution of tags for each simulation

  // Read in SEAPODYM data (if present)
  adstring spd_name = "../seapodym.dat";
  cifstream spd(spd_name);
  if (!spd)
    {
      clogf << "Unable to open seapodym.dat, " << spd_name << endl;
      seedtags = 1.0; // If no seapodym data, initialise to density of 1.0 for each cell
    }
  if(spd)
    {
      dmatrix spd_data(1, m, 1, n);
      for (int j=n; j>=1; j--)
	{
	  for (int i=1; i<=m; i++)
	    {
	      spd >> spd_data(i,j);
	    }
	}
      assignSeapodym(seedtags, spd_data); // Now use seapodym data to create initial distribution of tags 
    }
  
  dvector zoneCells(0, nzone);
  zoneCells = zonesum0;

  dvector cur_zs(0,nzone); 
  dvector prev_zs(0,nzone);
  dvector zone_half_life(0,nzone);
  dvector average_zone_half_life(0,nzone);
  average_zone_half_life.initialize();

  dmatrix tags(1, m, jlb, jub);
  dmatrix prev_tags(1, m, jlb, jub);
  dmatrix dmU(0, m + 1, jlb1, jub1); dmU.initialize();
  dmatrix dmV(0, m + 1, jlb1, jub1); dmV.initialize();
  dmatrix dmSigma(0, m + 1, jlb1, jub1); dmSigma.initialize();
  dmatrix dmTotMort(1, m, jlb, jub); dmTotMort.initialize();
  dmatrix half_life(1, m, jlb, jub);
  dmatrix average_half_life(1, m, jlb, jub); average_half_life.initialize();
  ivector ivEffortOccured(1, nfleet); ivEffortOccured.initialize();

  dmatrix densitySum(0, nzone, 0, nmonth); densitySum.initialize();

  /** Tag returns for which there IS reported effort).*/
  d3_array Recaps1 = make_d3_array(0, nfleet, 1, m, jlb, jub);
  Recaps1.initialize();
  /** Tag returns for which there is NO reported effort).*/
  d3_array Recaps0 = make_d3_array(0, nfleet, 1, m, jlb, jub);
  Recaps0.initialize();

  d3_array d3aEffort = make_d3_array(0, nfleet, 1, m, jlb, jub);
  d3aEffort.initialize();

  d3_array FishMort1 = make_d3_array(0, nfleet, 1, m, jlb, jub);
  FishMort1.initialize();
  d3_array FishMort0 = make_d3_array(0, nfleet, 1, m, jlb, jub);
  FishMort0.initialize();

  d3_array d3aRelease = make_d3_array(0, nfleet, 1, m, jlb, jub);
  d3aRelease.initialize();
 
#ifdef __BCG_SOLVER__
  linbcg bcg(*this);
  bcg.set_tol(1e-8);
  clogf << "\nUsing biconjugate gratient solver" << endl;
  clogf << "tol = " << bcg.get_tol() << endl;
#else
  interflag.setsaveinter(0);
  Upwind_coff_t<d3_array,dmatrix,dvector,double> coff(*this, m_ipar[6]);
  clogf << "\nUsing adi solver" << endl;
#endif
 
  // Loop through zones to remove tags from 
  for (int zn = 0; zn <= max(zonemap); zn++)
  {
  if(zoneCells(zn) <= 0.0) continue; // only want simulations excluding tags from regions that exist in model domain
  cout << "Starting densitysim, with tags excluded from region " << zn << endl;
  
  int numCurrentSeason = 0;
  double curr_average = 0.0; 
  double prev_average = curr_average;
 
  tags = 0.0;
  densitySum = 0.0;

  for (int start_month = 1; start_month <= 12; start_month++)
  {
    double curr_time = 0.0;
    double prev_time = curr_time;
    zonesum0.initialize();

    year_month start_date(1,start_month); // average effort starts in year 1
    year_month final_date = start_date + nmonth - 1;

    startingDensity(tags, seedtags); // Initialize tags to seedtags
    prev_tags = tags;
    half_life = -1.0;
    zone_half_life = -1.0;


    // Remove tags for specific regions (as required)
    remove_tags(tags, zonemap, zn);
    
    densitycomp(tags, zonemap, 0, densitySum); // Add initial tag density to cumulative total
    initialDensity(tags, zonemap, zonesum0); //Recalculate zonesum0 - sum of initial density by zone (accounting for removal of tags from zone zn)

    initial_prev_zs(zonesum0, prev_zs); // Assign starting values to prev_zs - required for estimation of halflives < 1 month
    
    for (year_month date = start_date; date <= final_date; date++)
    {
      int numMonthSeason = get_season(date);
   
      //season change redo uvs
      if (numCurrentSeason != numMonthSeason)
      {
        numCurrentSeason = numMonthSeason;
        uvs_comp(dmU, dmV, dmSigma, date);
      #ifdef __BCG_SOLVER__
        bcg.comp(*this, dmU, dmV, dmSigma);
      #else
        coff.comp(*this, dmU, dmV, dmSigma);
      #endif
      }//ifnumMonthSeason

      int effort_month = date.get_month_value();
      global_irfr.get_average_effort_array(effort_month, d3aEffort);

      //fish_mort_comp(d3aFishMort, d3aEffort, date);//mort=q*effort
      fish_mort_comp(FishMort1, FishMort0, d3aEffort, date, d3aRelease(0), Recaps0, global_irfr.get_mean_effort() );//mort=q*effort
      total_mort_comp(FishMort1, FishMort0, dmTotMort);
    #ifdef __BCG_SOLVER__
      bcg.b_adjust(dmTotMort);
    #else
      coff.b_adjust(*this, dmTotMort);
    #endif


    // solve the PDE
    #ifdef __BCG_SOLVER__
      bcg.solver(tags, m_ipar6);
    #else
      coff.adi(tags, isp);
    #endif
      curr_time ++;

      halfcomp(tags, prev_tags, curr_time, prev_time, 0.5, seedtags, half_life); 
      halfcomp(tags, zonemap, zonesum0, prev_zs, cur_zs, 
               curr_time, prev_time, 0.5, zone_half_life);

      densitycomp(tags, zonemap, curr_time, densitySum); // Add tag density to cumulative total for the given time-step 

      prev_time = curr_time;

    } // for (year_month date = start_date; date <= final_date; date++)

   
    curr_average ++;
                                    
    update_average(half_life, average_half_life, curr_average, prev_average);
    update_average(zone_half_life, average_zone_half_life, 
                   curr_average, prev_average);
    
    // Catch instances where density in zone has not decreased by 50%..
    for (int k = 0; k <= max(zonemap); k++)
      {
	if (zoneCells(k) > 0.0 && average_zone_half_life(k) < 0.0 && k != zn)
	  {
	    cout << "Half-life not reached. Increase nmonth" << endl;
	    clogf << "\nHalf-life not reached for region " << setw(5) << k << "Start month " << setw(5) << start_month << "\nIncrease nmonth!!" << endl;
	  }
      }
    

    prev_average = curr_average;
    //TTRACE(half_life, average_half_life)

  } //for (int start_month = 1; start_month <= 12; start_month++)
  cout << "Finished date loop!" << endl;

  clogf << "\n\nOutput for simulations excluding region " << zn << endl;

  clogf << "\nAverage halflife by eez" << endl;
  for (int k = 0; k <= max(zonemap); k++)
  {
    if (zoneCells(k) > 0.0 && k != zn)
      clogf << setw(5) << k 
            << setw(15) << setprecision(3) << average_zone_half_life(k) << endl;
  }

  clogf << "\nTotal Density by eez (summed across simulations for all start months)" << endl;

    for(int i = 0; i <= max(zonemap); i++)
      {
	if(zoneCells(i) <= 0.0) continue;
	clogf << setw(5) << i;
	int j1 = densitySum(i).indexmin();
	int j2 = densitySum(i).indexmax();
	for(int j = j1; j <= j2; j++)
	  {
	    clogf << setw(10) << setprecision(4) << densitySum(i, j);
	  }
	clogf << endl;
	
      }
  } // zn loop

  //clogf << "\nInitial Density by eez (for a given start month)" << endl;
} //void par_t_reg::halflife(indexed_regional_fishery_record& irfr)
