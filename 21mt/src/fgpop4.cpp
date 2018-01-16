#include "policy.h"
#include "par_treg.h"
#include "trace.h"
int saveinterflag = 0;


// delay difference version

//#define __BCG_SOLVER__
#ifdef __BCG_SOLVER__
  #include "linbcg.h"
#else
  #include "coff_t.h"
  #include "intersav.h"
  extern setsaveinterflag interflag;
  extern intersavetype *isp;
#endif

#include <prnstrem.h>
#include "trace.h"

extern ofstream clogf;

double min(const dmatrix&);

void get_average_effort_array(par_t& param, indexed_regional_fishery_record& irfr,
                      d3_array& effort, year_month& date,
                      ivector& effort_occured);

int read_effort(par_t& param, d3_array& effort, year_month& date,
                ivector& effort_occured);
dmatrix slice_sum(const d3_array& t);

d3_array make_d3_array(int sl,int sh, int nrl, int nrh,
                       const ivector& ncl, const ivector& nch);
void catch_comp(par_t& param, dmatrix& pop, d3_array& rel_catch,
                dmatrix& z, d3_array& fish_mort, ivector& effort_occured);

extern adstring banner;

void par_t_reg::population_simulate(policy& pol, indexed_regional_fishery_record& irfr,
                                    JNIEnv *env, jobject jobj,
                                    const adstring& path, const adstring& file)
{
  cout << __FILE__ << ": par_t_reg::population_simulate" << endl;
#ifdef _show_sst_
  int recruitment_mode_flag_m_ipar = 131;
  int recruitment_mode_flag = m_ipar[recruitment_mode_flag_m_ipar];
#else
  int recruitment_mode_flag = 0;
#endif
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

  const int t = 3;
  d3_array B = make_d3_array(1,3, 1,m, jlb,jub);
  dmatrix& dmPop = B(t);
  population_initialize(dmPop,5e-4);
  B(t-1) = B(t);
  B(t-2) = B(t-1);
  HERE

  d3_array s = make_d3_array(1, 3, 1,m, jlb,jub);
  s=1.0;
  HERE

  // growth constants
  const double vbK = 0.33; // per year for yellowfin
  const double rho = exp(-vbK/12.0);

  dvector w(2,3);
  w(t)   = 1.45; // weight of 41.7 cm yellowfin
  w(t-1) = 1.07; // weight one month previously
  HERE


  
  d3_array R = make_d3_array(2, 3 , 1, m, jlb, jub);
  R.initialize();
  dmatrix& dmRecruitment = R(3);
  HERE
  dmatrix       dmU(0, m + 1, 0, n + 1); dmU.initialize();
  dmatrix       dmV(0, m + 1, 0, n + 1); dmV.initialize();
  dmatrix   dmSigma(0, m + 1, 0, n + 1); dmSigma.initialize();

  dmatrix       z(1, m, jlb, jub); z.initialize(); // for use in b_adjust onlu
  dmatrix   dmTotMort(1, m, jlb, jub); dmTotMort.initialize();

  dmatrix dmTotalPredCaps(1, m, jlb, jub); dmTotalPredCaps.initialize();
  dmatrix  dmTotalObsCaps(1, m, jlb, jub); dmTotalObsCaps.initialize();

  ivector ivEffortOccured(1, nfleet); ivEffortOccured.initialize();

  d3_array d3aEffort = make_d3_array(0, nfleet, 1, m, jlb, jub);
  d3aEffort.initialize();
  d3_array d3aFishMort = make_d3_array(0, nfleet, 1, m, jlb, jub);
  d3aFishMort.initialize();


 d3_array d3aRel_catch = make_d3_array(0, nfleet, 1,m, jlb,jub);
  d3aRel_catch.initialize();

  int max_months = nmonth+sum(pol.phase_duration);

  dvector dvMonthlyPop(1,2);
  dvMonthlyPop.initialize();
  dvector dvMonthlyCatch(1,nfleet+1);
  dvMonthlyCatch.initialize();

#ifdef __BCG_SOLVER__
  linbcg bcg(*this);
  clogf << "\nUsing biconjugate gratient solver" << endl;
#else
  interflag.setsaveinter(0);
  Upwind_coff_t coff(*this, m_ipar[6]);
  clogf << "\nUsing adi solver" << endl;
#endif

  int m_ipar96 = m_ipar[96],
      //m_ipar13 = m_ipar[13],
      //m_ipar16 = m_ipar[16],
      m_ipar68 = m_ipar[68],
      m_ipar6  = m_ipar[6];

#ifndef __jniGraphics_h__
  m_ipar68 = 0;
  HERE
#else
  double gridcells = 0;
  {
    for (int i = 1; i <= m; i++)
      for (int j = 1; j <= n; j++)
        if (gridmap(i,j) > 0)
        gridcells ++;
  }
  TRACE(gridcells)

  adstring coastline_file = path+"..\\"+file+".cst";
  TRACE(coastline_file)

  jniGraphics jni(env, jobj);

  RowColTab sstRCT(0,0,0);
  RowColTab mvtRCT(0,1,0);
  RowColTab recRCT(1,0,0);
  RowColTab popRCT(1,1,0);

  RowColTab fmortRCT(0,0,1);
  RowColTab catRCT(0,1,1);
  RowColTab pophistRCT(1,0,1);
  RowColTab cathistRCT(1,1,1);

  HERE
  if (!m_ipar68)
  {

    // setup genaral layout
    jni.addNumTabs(2);

    jni.setTitleTab("Monthly Population",0);
    jni.setGridLayout(2, 2, 0);
    jni.addSquareColorMap(sstRCT);
    jni.addRegionalMovementPattern(mvtRCT);
    jni.addSquareColorMap(recRCT);
    jni.addSquareColorMap(popRCT);

    jni.setTitleTab("Fishery History",1);
    jni.setGridLayout(2, 2, 1);
    jni.addSquareColorMap(fmortRCT);
    jni.addSquareColorMap(catRCT);
    jni.addLinePlot(pophistRCT);
    jni.addLinePlot(cathistRCT);

    jni.layoutAll();
    HERE

    // setup each tab
    double swlong = sw_coord.long_value();
    double swlat  = sw_coord.lat_value();
    TTRACE(swlong,swlat)
    TTRACE(m,n)
    TRACE(ngrid)

    // tab 0
    jni.setSquareColorMapTitle("SST", sstRCT);
    jni.setSquareColorMapMN(m, n, sstRCT);
    jni.setSquareColorMapFileImgMap(coastline_file, sstRCT);
    jni.setSquareColorMapBoundsImgMap(swlong, swlat, m, n, deltax, deltay, sstRCT);
    jni.setSquareColorMapUniformScalesLegend(21, 30, sstRCT);
    jni.setSquareColorMapNumDecimalPlacesLegend(0, sstRCT);
    jni.setSquareColorMapIncrementValueLegend(10, sstRCT);
    HERE

    jni.setRegionalMovementPatternTitle("Movement Pattern", mvtRCT);
    jni.setRegionalMovementPatternMN(m, n,  mvtRCT);
    jni.setRegionalMovementPatternFileImgMap(coastline_file,  mvtRCT);
    jni.setRegionalMovementPatternBoundsImgMap(swlong, swlat, m, n, deltax, deltay,  mvtRCT);
    jni.setRegionalMovementPatternNumGrid(ngrid,  mvtRCT);
    jni.setRegionalMovementPatternGridMap(gridmap,  mvtRCT);
    HERE;

    int recruitment_mode = m_ipar(131);
    adstring recruitment_mode_string;
    if (recruitment_mode == 0)
      recruitment_mode_string = "Recruitment (uniform)";
    else if (recruitment_mode == 3)
      recruitment_mode_string = "Recruitment (rectangle)";
    else if (recruitment_mode == 4)
    {
      recruitment_mode_string = "Recruitment (SST dependent)";
    }
    else
    {
      recruitment_mode_string = "Recruitment (unspecified mode)";
    }

    jni.setSquareColorMapTitle(recruitment_mode_string, recRCT);
    jni.setSquareColorMapMN(m, n, recRCT);
    jni.setSquareColorMapFileImgMap(coastline_file, recRCT);
    jni.setSquareColorMapBoundsImgMap(swlong, swlat, m, n, deltax, deltay, recRCT);
    jni.setSquareColorMapNumDecimalPlacesLegend(0, recRCT);
    jni.setSquareColorMapIncrementValueLegend(1, recRCT);
    jni.setSquareColorMapUniformScalesLegend(0.0001, 1.0, recRCT);
    HERE;

    jni.setSquareColorMapTitle("Population Density", popRCT);
    jni.setSquareColorMapMN(m, n, popRCT);
    jni.setSquareColorMapFileImgMap(coastline_file, popRCT);
    jni.setSquareColorMapBoundsImgMap(swlong, swlat, m, n, deltax, deltay, popRCT);
    jni.setSquareColorMapNumDecimalPlacesLegend(0, popRCT);
    jni.setSquareColorMapIncrementValueLegend(1, popRCT);
    jni.setSquareColorMapLogScalesLegend(0.001, 5.0*(double)m_ipar(61)/gridcells, popRCT);
    HERE;

    // tab 1
    jni.setSquareColorMapTitle("Fishing Mortality", fmortRCT);
    jni.setSquareColorMapMN(m, n, fmortRCT);
    jni.setSquareColorMapFileImgMap(coastline_file, fmortRCT);
    jni.setSquareColorMapBoundsImgMap(swlong, swlat, m, n, deltax, deltay, fmortRCT);
    jni.setSquareColorMapNumDecimalPlacesLegend(1, fmortRCT);
    jni.setSquareColorMapIncrementValueLegend(0.1, fmortRCT);
    jni.setSquareColorMapUniformScalesLegend(0.001, 0.5, fmortRCT);
    HERE;

    jni.setSquareColorMapTitle("Total Catch", catRCT);
    jni.setSquareColorMapMN(m, n, 0, 1, 1);
    jni.setSquareColorMapFileImgMap(coastline_file, catRCT);
    jni.setSquareColorMapBoundsImgMap(swlong, swlat, m, n, deltax, deltay, catRCT);
    jni.setSquareColorMapNumDecimalPlacesLegend(0, catRCT);
    jni.setSquareColorMapIncrementValueLegend(1, catRCT);
    jni.setSquareColorMapLogScalesLegend(0.001, 1.0, catRCT);
    HERE;

    jni.setLinePlotCountXaxis(max_months, pophistRCT);
    jni.setLinePlotCountYaxis(5, pophistRCT);
    jni.setLinePlotRangeYaxis(0.0, (double)m_ipar(61), pophistRCT);
    jni.setLinePlotNumLines(2, pophistRCT);
    jni.setLinePlotIncrementXaxis(5, pophistRCT);
    jni.setLinePlotIncrementYaxis(1, pophistRCT);
    jni.setLinePlotPrecisionYaxis(1, pophistRCT);
    HERE

    jni.setLinePlotCountXaxis(max_months, cathistRCT);
    jni.setLinePlotCountYaxis(5, cathistRCT);
    jni.setLinePlotRangeYaxis(0.0, 2.0, cathistRCT);
    jni.setLinePlotNumLines(nfleet+1, cathistRCT);
    jni.setLinePlotIncrementXaxis(5, cathistRCT);
    jni.setLinePlotIncrementYaxis(1, cathistRCT);
    jni.setLinePlotPrecisionYaxis(1, cathistRCT);
    HERE

    jni.paintAll();
    HERE
  }
#endif

  int numCurrentSeason = 0;


  /*
  int start_month = 1;
  adstring pop_file_name("equilb.pop");

  int read_pop_file =  file_exists(pop_file_name) && param.ipar[100];
  int write_pop_file = 1;
  TRACE(file_exists(pop_file_name))
  TRACE(param.ipar[100])
  TRACE(read_pop_file)
  */
  prnstream fp("fishing.prn");
  fp << (char*)banner << PENDL;
  fp << "   " << "Fishing effort by fleet:" << PENDL;
  fp << "Month";

  prnstream pp("popsim.prn");
  pp << (char*)banner << PENDL;
  pp << "     " << "Total"       << "Spawning" << "Total"       << "Total" << "catch by fleet" << PENDL;
  pp << "Month" << "Recruitment" << "Biomass"  << "Popuplation" << "Catch";

  for (int f = 1; f <= nfleet; f++)
  {
    pp << (char*)fleet_names(f);
    fp << (char*)fleet_names(f);
  }
  pp << PENDL;
  fp << PENDL;

  double spawning_biomass = 0.0;
  year_month date = start_date - 1;
  int elapsed_month = 0;
  int nphase = pol.nphase;
  //pp << elapsed_month << sum(dmRecruitment) << sum(dmPop) << sum(d3aRel_catch) << PENDL;

  for (int phase = 0; phase <= nphase; phase++)
  {
    int m2 = 0;
    int m1 = m2 + 1;
    if (phase == 0)
      m2 = nmonth;
    else
      m2 = pol.phase_duration(phase);

    int months_in_phase = 0;
    for (int mm = m1; mm <= m2; mm ++)
    {
      B(t-2) = B(t-1);
      B(t-1) = B(t);
      HERE
      R(t-1) = R(t);
      HERE
      s(t-2) = s(t-1);
      s(t-1) = s(t);
      HERE
      TTRACE(min(B(t)),max(B(t)))
      B(t) = elem_prod(s(t-1),B(t-1))*(1.0+rho)
           - rho*elem_prod( elem_prod(s(t-1),s(t-2)), B(t-2) )
           - rho*w(t-1)*elem_prod(s(t-1),R(t-1))
           + w(t)*R(t);
      TTRACE(min(B(t)),max(B(t)))
      HERE
      date ++;
      elapsed_month ++;
      months_in_phase ++;
      cout << phase << " " << date << ": " << elapsed_month << endl;

      int numMonthSeason = get_season(date);

      //season change redo uvs
      if (numCurrentSeason != numMonthSeason)
      {
        //if (  (date == start_date)
        //   || (elapsed_month > nmonth/2) )
        //{
        numCurrentSeason = numMonthSeason;

        if (!m_ipar96)
          uvs_comp(dmU, dmV, dmSigma, date);

      #ifdef __BCG_SOLVER__
        bcg.comp(*this, dmU, dmV, dmSigma);
      #else
        coff.comp(*this, date, isp, dmU, dmV, dmSigma);
      #endif
        //}

      } //if (numCurrentSeason != numMonthSeason)

      recruit_comp(dmRecruitment, dmPop, date, spawning_biomass);
      //dmPop = dmPop + dmRecruitment;
      //bcg.g_comp(*this, dmRecruitment);

      if (phase >0) // NEVER phish during phase 0
      {
        //if (m_ipar16 > 0)
        //{
          // what?
        //}
        //else
        //{
        pol.get_effort_array(*this, irfr, d3aEffort, date, ivEffortOccured,
                               phase, months_in_phase);
        //}
        pol.fish_mort_comp(d3aFishMort, d3aEffort);
        //dmatrix dmTotalFishMort = slice_sum(d3aFishMort);
      }
      total_mort_comp(d3aFishMort, dmTotMort);
      s(t) = exp(-1.0*dmTotMort); //1.0 - dmTotMort;
    #ifdef __BCG_SOLVER__
      bcg.b_adjust(z);
    #else
      coff.b_adjust(*this, z);
    #endif

      double total_catch = 0.0;
      if (phase >0)
      {
        catch_comp(*this, dmPop, d3aRel_catch, dmTotMort, d3aFishMort, ivEffortOccured);
        d3aRel_catch(0).initialize();
        for (int fl = 1; fl <= nfleet; fl++)
        {
          dvMonthlyCatch(fl) = sum(d3aRel_catch(fl));
          d3aRel_catch(0)+= d3aRel_catch(fl);
          total_catch +=dvMonthlyCatch(fl);
        }
        dvMonthlyCatch(nfleet+1) =total_catch;
      }

      double total_pop = sum(dmPop);
      dvMonthlyPop(1) = total_pop;
      dvMonthlyPop(2) = spawning_biomass;


      pp << elapsed_month << sum(dmRecruitment) << spawning_biomass
         << total_pop << total_catch;
      fp << elapsed_month;
      for (int fl = 1; fl <= nfleet; fl++)
      {
        pp << dvMonthlyCatch(fl);
        fp << sum(d3aEffort(fl));
      }
      pp << PENDL;
      fp << PENDL;

#ifdef __jniGraphics_h__
    if (!m_ipar68)
    {
      char buf[80];
      ostrstream ss(buf,80);
      ss << "Month " << setw(3) << setfill(' ') << dec << elapsed_month
         << ", " << date
         << ", season " << numCurrentSeason
         << ", fishery development phase " << phase
         << '\0';
      cout << buf << endl;
      jni.setStatusBarDisplay(buf);

      dmatrix dmTotalFishMort = slice_sum(d3aFishMort);
      dmatrix sst(1,m,jlb,jub);
      int month = date.get_month_value();
      const int oblen = 20;
      char ob2[oblen];
      ostrstream sob2(ob2,oblen);
    #ifdef unix
      sob2 << "../nodc00" << setw(2) << setfill('0') << month  << ".sst" << '\0';
    #else
      sob2 << "..\\nodc00" << setw(2) << setfill('0') << month  << ".sst" << '\0';
    #endif
      ifstream sstf(ob2);
      sstf >> sst;
      /*
      for (int i = 1; i <= m; i++)
      {
        int j1 = jlb(i);
        int j2 = jlb(i);
        for (int j = j1; j <= j2; j++)
        {
          if (gridmap(i,j) == 0)
            sst(i,j) = 0.0;
        }
      }
      */
      jni.drawSquareColorMapSquares(sst, ilb, iub, sstRCT);
      int s = numCurrentSeason;
      jni.drawRegionalMovementPatternBase(mvtRCT);
      jni.drawRegionalMovementPatternVectors(get_seasonal_u(s), get_seasonal_v(s),
                                                mvtRCT);
      jni.drawRegionalMovementPatternSigmas(get_seasonal_sigma(s), mvtRCT);
      jni.drawSquareColorMapSquares(dmRecruitment, ilb, iub, recRCT);
      jni.drawSquareColorMapSquares(dmPop, ilb, iub, popRCT);

      jni.drawSquareColorMapSquares(dmTotalFishMort, ilb, iub, fmortRCT);
      jni.drawSquareColorMapSquares(d3aRel_catch(0), ilb, iub, catRCT);
      jni.drawLinePlotPoints(dvMonthlyPop, pophistRCT);
      jni.drawLinePlotPoints(dvMonthlyCatch, cathistRCT);

      jni.paintAll();
    }//ifm_ipar68
#endif

    #ifdef __BCG_SOLVER__
      bcg.solver(dmPop, m_ipar6);
    #else
      coff.adi(dmPop, isp);
    #endif
      TTRACE(min(dmPop),max(dmPop))

     } //for (year_month date = start_date; date <= final_date; date++)
  } //for (int phase = 0; phase <= nphase; phase++)
}//End par_t_reg::population_simulate(JNIEnv*, jobject)
