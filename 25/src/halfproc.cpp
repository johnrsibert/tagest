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
using std::ostringstream;

#ifdef __BCG_SOLVER__
  #include "linbcg.h"
#else
  #include "coff_t.h"
  #include "intersav.h"
  extern setsaveinterflag interflag;
  extern intersavetype *isp;
#endif
extern indexed_regional_fishery_record global_irfr;

void halfcomp(const dmatrix& density, const imatrix map, dvector& sum0, 
              dvector& prev_sum, dvector& cur_sum, 
              const double cur_time, double& prev_time,
              const double half, dvector& half_life);
void update_average(const dvector& current, dvector& average, 
                    const double& curr_time, double& prev_time);
void halfcomp(const dmatrix& density, dmatrix& prev_density, 
              const double curr_time, double& prev_time,
              const double half, 
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
template <>
void par_t_reg<d3_array,dmatrix,dvector,double>::halflife(indexed_regional_fishery_record& irfr)
{
  cout << __FILE__ << ": par_t_reg::halflife" << endl;
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
  int average_plot = 0;

  TRACE(graphics_on)
  if (graphics_on)
  {
    TRACE(jni)
    jni = new jnigraphics2();
    if (!jni)
    {
      cerr << "Error creating instance of jnigraphics object. Exiting." << endl;
      exit(1);
    }

    jni->addTabs(ntabs, tab_list);
    monthly_tab = tab_list(1);
    average_tab = tab_list(2);
    density_tab = tab_list(3);

    jni->setTabTitle("Halflife by Start Month",monthly_tab);
    int tcol = 4;
    int trow = 3;
    if (m > n)
    {
      tcol = 3;
      trow = 4;
    }
    mt_layout.allocate(1, trow*tcol);
    jni->addGridLayout(trow, tcol, mt_layout, monthly_tab);
    td_layout.allocate(1, trow*tcol);
    jni->addGridLayout(trow, tcol, td_layout, density_tab);

    jni->addStatusBar();

    squareregiondef scm(m, n);
    scm.region.dx = deltax;
    scm.region.dy = deltay;
    scm.region.setGridMap(gridmap);
    scm.region.cstfile = coastline_file;
    scm.legend.min = 0.0;
    scm.legend.max = 12.0;
    scm.legend.increment = 1;
    scm.legend.decimals = 0;
    scm.region.title=(char*)"Start Month 00";
    int ij = 0; 
    for (int i = 1; i <= tcol; i++)
    {
      for (int j = 1; j <= trow; j++)
      {
        ij ++;
        ostringstream rss;
        //rss << "Start Month " << ij;
        rss << "Start Month " << setw(2) << setfill('0') << ij << ends;
        //rss << ij;
        TTRACE(scm.region.title,rss.str().c_str())
        scm.region.title = (char*)rss.str().c_str();
        TTRACE(scm.region.title,rss.str().c_str())
        jni->addSquareRegion(scm, mt_layout(ij));
      }
    }

    jni->setTabTitle("Average Halflife",average_tab);
    ivector at_layout(1, 1);
    jni->addGridLayout(1, 1, at_layout, average_tab);
    average_plot = at_layout(1);
    squareregiondef sca(m, n);
    sca.region.dx = deltax;
    sca.region.dy = deltay;
    sca.region.setGridMap(gridmap);
    sca.region.cstfile = coastline_file;
    sca.region.title = (char*)"--- Average Halflife ---";
    sca.legend.min = 0.0;
    sca.legend.max = 24.0;
    sca.legend.increment = 2;
    sca.legend.decimals = 0;
    jni->addSquareRegion(sca, average_plot);

    jni->setTabTitle("Tag Density [0,1]",density_tab);
    squareregiondef scd(m, n);
    scd.region.dx = deltax;
    scd.region.dy = deltay;
    scd.region.setGridMap(gridmap);
    scd.region.cstfile = coastline_file;
    scd.legend.min = 0.0;
    scd.legend.max = 1.0;
    scd.legend.increment = 0.1;
    scd.legend.decimals = 1;
    ij = 0; 
    for (int i = 1; i <= tcol; i++)
    {
      for (int j = 1; j <= trow; j++)
      {
        ij ++;
        ostringstream rss;
        //rss << "Start Month " << ij;
        rss << "Start Month " << setw(2) << setfill('0') << ij << ends;
        //rss << ij;
        scd.region.title = (char*)rss.str().c_str();
        //TRACE(rss.str().c_str())
        //strcpy(scd.region.title,rss.str().c_str());
        TTRACE(scd.region.title,rss.str().c_str())
        jni->addSquareRegion(scd, td_layout(ij));
      }
    }

    jni->layoutAll(); //800,600);
  }
  //char junk;
  //cin >> junk; 
  //  exit(1);
  HERE

  imatrix zonemap(1,m,1,n);
  adstring zm_name = "../"+pathname.getPathRoot()+".eez";
  cifstream zf(zm_name);
  if (!zf)
  {
    cerr << "Unable to open eez map file, " << zm_name << endl;
    exit(1);
  }
  int nzone = 100; //max(zonemap);
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

  HERE
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


  int numCurrentSeason = 0;
  double curr_average = 0.0; 
  double prev_average = curr_average;
  for (int start_month = 1; start_month <= 12; start_month++)
  {
    double curr_time = 0.0;
    double prev_time = curr_time;
    zonesum0.initialize();

    // initialize tag density to 1.0
    tags = 1.0;
    prev_tags = tags;
    year_month start_date(1,start_month); // average effort starts in year 1
    year_month final_date = start_date + nmonth;
    //half_life.initialize();
    half_life = -1.0;
    zone_half_life = -1.0;
 
    for (year_month date = start_date; date <= final_date; date++)
    {
      int numMonthSeason = get_season(date);
      //char sbuf[80];
      ostringstream ss; //(sbuf,80);
      ss << " Fit p" << setw(2) << setfill('0') << hex << m_ipar[8]
         << ": " << date
         << ", Season " << numMonthSeason
         << ", Start month " << setw(2) << setfill(' ') << dec << start_month
         << ends;
      //cout << sbuf << endl;
   
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
      year_month effort_date(1,effort_month);
      get_effort_array(*this, irfr, d3aEffort, effort_date, ivEffortOccured);
      //fish_mort_comp(d3aFishMort, d3aEffort, date);//mort=q*effort
      fish_mort_comp(FishMort1, FishMort0, d3aEffort, date, d3aRelease(0), Recaps0, global_irfr.get_mean_effort() );//mort=q*effort
      total_mort_comp(FishMort1, FishMort0, dmTotMort);
    #ifdef __BCG_SOLVER__
      bcg.b_adjust(dmTotMort);
    #else
      coff.b_adjust(*this, dmTotMort);
    #endif

      if (graphics_on)
      {
        jni->drawStatusBar((char*)ss.str().c_str());

        //char jbuf[80];
        ostringstream jss; //(jbuf,80);
      #ifdef unix
        jss << "jpeg/"
      #else
        jss << "jpeg\\"
      #endif
            << "td" << setw(2) << setfill('0') << hex << m_ipar[8] << dec
                    << setw(2) << setfill('0') << date.get_year()
                    << setw(2) << setfill('0') << date.get_month_value()
                    << ".jpg" << ends;

        jni->drawSquareRegion(half_life, mt_layout(start_month));
        jni->drawSquareRegion(tags, td_layout(start_month));

        if ( !(jni->paintAll()) )
          exit(0);
      }//if (graphics_on)


    // solve the PDE
    #ifdef __BCG_SOLVER__
      bcg.solver(tags, m_ipar6);
    #else
      coff.adi(tags, isp);
    #endif
      curr_time ++;
      halfcomp(tags, prev_tags, curr_time, prev_time, 0.5, half_life); 
      halfcomp(tags, zonemap, zonesum0, prev_zs, cur_zs, 
               curr_time, prev_time, 0.5, zone_half_life);
      prev_time = curr_time;

    } // for (year_month date = start_date; date <= final_date; date++)
    curr_average ++;
                                    
    update_average(half_life, average_half_life, curr_average, prev_average);
    update_average(zone_half_life, average_zone_half_life, 
                   curr_average, prev_average);
    prev_average = curr_average;
    //TTRACE(half_life, average_half_life)
    if (graphics_on)
    {
      jni->drawSquareRegion(average_half_life, average_plot);
    }
  } //for (int start_month = 1; start_month <= 12; start_month++)
  if (graphics_on)
  {
    jni->drawStatusBar(adstring(" Finished"));
  }
  cout << "Finished date loop!" << endl;
  clogf << "\nAverage halflife by eez" << endl;
  for (int k = 0; k <= nzone; k++)
  {
    if (average_zone_half_life(k) > 0.0)
      clogf << setw(5) << k 
            << setw(15) << setprecision(3) << average_zone_half_life(k) << endl;
  }
  /*
  if (graphics_on)
  {
    char fbuf[80];
    ostrstream fss(fbuf,80);
  #ifdef unix
    fss << "jpeg/"
  #else
    fss << "jpeg\\"
  #endif
        << "final" << setw(2) << setfill('0') << hex << m_ipar[8] << dec
        << ".jpg" << ends;
    jni->savejpeg(fbuf, cop_plot);
    TRACE(fbuf)

    char hbuf[80];
    ostrstream hss(hbuf,80);
  #ifdef unix
    hss << "jpeg/"
  #else
    hss << "jpeg\\"
  #endif
        << "history" << setw(2) << setfill('0') << hex << m_ipar[8] << dec
        << ".jpg" << ends;
   jni->savejpeg(hbuf, history_plot);
   TRACE(hbuf)
  } // if (graphics_on)
  HERE
 */

  if (graphics_on)
  {
    // wait untile the graphics system is closed
    while ( (jni->paintAll()) ) {}
    exit(0);
  }

} //void par_t_reg::halflife(indexed_regional_fishery_record& irfr)
