#include <prnstrem.h>
#include "par_t_nn.h"
#include "trace.h"
int saveinterflag = 0;

extern indexed_regional_fishery_record global_irfr;
#include <jnigraphics2.h>
#ifdef __BCG_SOLVER__
  #include "linbcg.h"
#else
  #include "coff_t.h"
  #include "intersav.h"
  extern setsaveinterflag interflag;
  extern intersavetype *isp;
#endif


void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr,
       d3_array& effort, year_month& date,
       ivector& effort_occured);

int read_effort(par_t& param, d3_array& effort, year_month& date,
                ivector& effort_occured);

dmatrix slice_sum(const d3_array& t);
void obs_recap_comp(dmatrix& total_obs_caps, year_month& date, int cohort,
                    recaptype_vector& recaps, int nrec, par_t& param,
                    ivector& effort_occured);

void capcomp(year_month& date, int cc, par_t& param, dmatrix& tags,
             ofstream& capfile, double& tcaps, dvector& region_caps,
             dmatrix& total_mort, d3_array& fish_mort, ivector& effort_occured,
             dvector& pred_tac, float f_unused, int i_unused, dmatrix& total_pred_caps);

void save_tags(par_t& param, dmatrix& releases, year_month& date,
               ofstream& savefile);

void total_check_comp(dvector& total_tags, dmatrix& total_check,
                      int month, dmatrix& release, par_t& param,
                      dmatrix& tot_mort);

dvector displacement_bins(par_t& param, dmatrix& distances);

void displacement_frequency(const int cohort, const dmatrix& tags,
                        dvector& displacements, dvector& xdisp, dvector& ydisp,
                        const dmatrix& distances, const dvector& bins,
                        par_t& param);

void write_displacement_files(const dmatrix& displacement,
                         dmatrix& xdisp, dmatrix& ydisp,
                         const dvector& bins,
                         const int fit_number, const char op);

d3_array make_d3_array(int sl,int sh, int nrl, int nrh,
                       const ivector& ncl, const ivector& nch);

double tag_scaling_factor = 1.0;

extern ofstream clogf;

void par_t_nn::simulate(JNIEnv* _env, jobject _jobj,
                         recaptype_vector& recaps, int nrec,
                         const adstring& path, const adstring& file)
{
      cout << __FILE__ << ": par_t_nn::simulate" << endl;
      int m_ipar96 = m_ipar[96],
      pool_tags = m_ipar[13],   //Pool tags
      m_ipar16 = m_ipar[16],
      m_ipar67 = m_ipar[67],
      m_ipar68 = m_ipar[68],  // graphics off 
      m_ipar6 = m_ipar[6];

  int graphics_on = m_ipar68;
  TRACE(graphics_on)
  
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
  adstring coastline_file = path+"../"+file+".cst";
#else
  adstring coastline_file = path+"..\\"+file+".cst";
//  adstring coastline_file = "..\\"+pathname.getPathRoot()+".1.2.cst";
#endif
  TRACE(coastline_file)

  int max_month = tr[nrelease].date - tr[1].date + nmonth + 1;
  if (max_month < 60)
    max_month = 60;
  TRACE(max_month)

  jnigraphics2* jni = NULL;
  TRACE(jni)

  const int ntabs = 5;
  ivector tab_list(1,ntabs);
  
  //int hr_plot = 0;    //harvest ratio
  int td_plot = 0;    // tag_density field
  int rm_plot = 0;    //movement pattern uv  resultant overlaid with sigma
  int op_plot = 0;    //observed and predicted tag return
  
  int cop_plot = 0;  //cumulative returns
  
  int history_plot = 0;    //obs and predicted recovery
  
  int dif_plot = 0;   //diffusion field
  int u_plot = 0;    //u-field
  int v_plot = 0;  //v-field

   int z_plot = 0;     //depth
  int dz_plot = 0;    //depth difference
  int fad_plot = 0;   //no.of fads
  int dst_plot = 0;   //distance from land
  int swc_plot = 0; //seasonal wind_curls
  int seas_curl_plot = 0; //seasonal wind
  int wcurl_plot = 0;  //wind_curl field
 
  //graphics_on = 1;  //default value !!
  TRACE(graphics_on)
  if (graphics_on)
  {
    TRACE(jni)
    #ifdef unix
        jni = new jnigraphics2("/home/aku/jancheta/jnigraphics3/g++");
   #else
        jni = new jnigraphics2("\\movemod\\jnigraphics3\\bcc55");
    #endif
     
   TRACE(jni)
    if (!jni)
    {
      cerr << "Error creating instance of jnigraphics object. Exiting." << endl;
      exit(1);
    }

    jni->addTabs(ntabs, tab_list);
    int monthly_tab = tab_list(1);
    int cumulative_tab = tab_list(2);
    int history_tab = tab_list(3);
    int movement_tab = tab_list(4);
    int input_tab = tab_list(5);

    jni->setTabTitle("Monthly",monthly_tab);
    ivector mt_layout(1, 3);
    if (n > m)
      jni->addGridLayout(1, 3, mt_layout, monthly_tab);
    else // makes large screens for SW Pacific
      jni->addGridLayout(2, 2, mt_layout, monthly_tab);
    //hr_plot = mt_layout(1);// = mtp[0];
    td_plot = mt_layout(1);// = mtp[1];
    rm_plot = mt_layout(2);// = mtp[2];
    op_plot = mt_layout(3);//= mtp[3];
    jni->addStatusBar();

    squareregiondef scm(m, n);
   
    /*
    scm.region.dx = deltax;
    scm.region.dy = deltay;
    scm.region.title = "Harvest Ratio";
    scm.region.setGridMap(gridmap);
    scm.region.cstfile = coastline_file;
    scm.legend.min = 0.001;
    scm.legend.max = 1.0;
    scm.legend.increment = 0.2;
    scm.legend.decimals = 1;
    jni->addSquareRegion(scm, hr_plot);
    */
    
    scm.region.title = "Tag Density";
    //scm.region.setGridMap(gridmap);
    scm.region.cstfile = coastline_file;
    scm.legend.min = 1.0;
    scm.legend.max = 1024.0;
    scm.legend.type = 1;
    scm.legend.increment = 2;
    scm.legend.decimals = 0;
    jni->addSquareRegion(scm, td_plot);
    
    
    scm.region.title = "Movement Pattern";
    //scm.region.setGridMap(gridmap);
    scm.legend.min = 1.0;
    scm.legend.max = maxsig; // 2500.0;
    scm.legend.type = 1;
    scm.legend.increment = 10.0;
    scm.legend.decimals = 0;
    jni->addSquareRegion(scm, rm_plot);
    HERE
        
    triangleregiondef tcm(m, n);
    tcm.region.dx = deltax;
    tcm.region.dy = deltay;
    tcm.region.title ="Observed/Predicted Returns";
    //tcm.region.setGridMap(gridmap);
    tcm.region.cstfile = coastline_file;
    tcm.legend.min = 1.0;
    tcm.legend.max = 256;
    tcm.legend.type = 1;
    tcm.legend.increment = 2;
    jni->addTriangleRegion(tcm, op_plot);
    
    jni->setTabTitle("Cumulative Returns",cumulative_tab);
    ivector ct_layout(1, 1);
    jni->addGridLayout(1, 1, ct_layout, cumulative_tab);
    //triangleregiondef ctcm(m, n);
    cop_plot = ct_layout(1);// = ctlp[0];
    //ctcm.region.dx = deltax;
    //ctcm.region.dy = deltay;
    tcm.region.title ="Observed/Predicted Returns";
    //tcm.region.setGridMap(gridmap);
    //tcm.region.cstfile = coastline_file;
    tcm.legend.min = 1.0;
    tcm.legend.max = 512;
    tcm.legend.type = 1;
    tcm.legend.increment = 2;
    jni->addTriangleRegion(tcm, cop_plot);

    jni->setTabTitle("History", history_tab);
    ivector ht_layout(1,1);
    jni->addGridLayout(1, 1, ht_layout, history_tab);
    history_plot = ht_layout(1);//= htlp[0];
  
    lineplotdef hpd(2);
    //hpd.title = "Tag Return History";
    hpd.xaxis = "Months After First Tag Release";
    hpd.xmax = ((max_month/5)+1)*5;
    hpd.xincrement = 5;
    hpd.yaxis = "Tag Returns";
    hpd.ymax = 600.0;
    hpd.yincrement = 100.0;
    hpd.setColor(255,0,0,0);
    hpd.setColor(0,0,255,1);
    jni->addLinePlot(hpd, history_plot);

    jni->setTabTitle("Movement Fields", movement_tab);
    ivector mv_layout(1,3);
    jni->addGridLayout(1, 3, mv_layout, movement_tab);

    u_plot = mv_layout(1);
    scm.region.title = "Advection Field (u)";
    //scm.region.setGridMap(gridmap);
    scm.legend.min = -2.0;
    scm.legend.max =  2.0;
    scm.legend.type = 0;
    scm.legend.increment = 1.0;
    scm.legend.decimals = 1;
    jni->addSquareRegion(scm, u_plot);

    v_plot = mv_layout(2);
    scm.region.title = "Advection Field (v)";
    //scm.region.setGridMap(gridmap);
    scm.legend.min = -2.0;
    scm.legend.max =  2.0;
    scm.legend.type = 0;
    scm.legend.increment = 1.0;
    scm.legend.decimals = 1;
    jni->addSquareRegion(scm, v_plot);

    dif_plot = mv_layout(3);
    scm.region.title = "Diffusion Field";
    //scm.region.setGridMap(gridmap);
    scm.legend.min = 1.0;
    scm.legend.max = maxsig; //1500.0;
    scm.legend.type = 1;
    scm.legend.increment = 10.0;
    scm.legend.decimals = 0;
    jni->addSquareRegion(scm, dif_plot);


    squareregiondef scm2(m, n);
    scm2.region.setGridMap(gridmap);
    scm2.region.cstfile = coastline_file;
    jni->setTabTitle("Input Layer", input_tab);
    ivector it_layout(2,3);
    jni->addGridLayout(2,3, it_layout, input_tab);
   
    z_plot   = it_layout(1);
    dz_plot = it_layout(2);
    fad_plot = it_layout(3);
    dst_plot = it_layout(4);
    swc_plot = it_layout(5);
    wcurl_plot = it_layout(6);
    

    z_plot = it_layout(1);
    scm2.region.title = "Water Depth";
    scm2.legend.min = -1.0;
    scm2.legend.max = 1.0;
    scm2.legend.type = 0;
    scm2.legend.increment = 0.01;
    scm2.legend.decimals = 2;
    jni->addSquareRegion(scm2, z_plot);

    dz_plot = it_layout(2);
    scm2.region.title = "Depth Difference";
    scm2.legend.min =    -1.0;
    scm2.legend.max =  1.0;
    scm2.legend.type = 0;
    scm2.legend.increment = 0.01;
    scm2.legend.decimals = 2;
    jni->addSquareRegion(scm2, dz_plot);

    fad_plot = it_layout(3);
    scm2.region.title = "Numbers of FADs";
    scm2.legend.min = -1.0;
    scm2.legend.max = 1.0;
    scm2.legend.type = 0;
    scm2.legend.increment = 0.01;
    scm2.legend.decimals = 2;
    jni->addSquareRegion(scm2, fad_plot);
   
    dst_plot = it_layout(4);
    scm2.region.title = "Distance from land";
    scm2.legend.min = -1.0;
    scm2.legend.max = 1.0;
    scm2.legend.type = 0;
    scm2.legend.increment = 0.01;
    scm2.legend.decimals = 2;
    jni->addSquareRegion(scm2, dst_plot);
 
    swc_plot = it_layout(5);
    scm2.region.title = "Seasonal  WStress Curl";
    scm2.legend.min = -1.0;
    scm2.legend.max = 1.0;
    scm2.legend.type = 0;
    scm2.legend.increment = 0.01;
    scm2.legend.decimals = 2;
    jni->addSquareRegion(scm2, swc_plot);
    
    wcurl_plot = it_layout(6);
    scm2.region.title = "Wind Stress Curl";
    scm2.legend.min =  -1.0;
    scm2.legend.max = 1.0;
    scm2.legend.type = 0;
    scm2.legend.increment = 0.01;
    scm2.legend.decimals = 3;
    jni->addSquareRegion(scm2, wcurl_plot);
 
    jni->layoutAll();

  }

  dvector history(1,2); history.initialize(); // for line plots

  dmatrix     dmU(0, m + 1, jlb1, jub1); dmU.initialize();
  dmatrix     dmV(0, m + 1, jlb1, jub1); dmV.initialize();
  dmatrix     dmSigma(0, m + 1, jlb1, jub1); dmSigma.initialize();
  dmatrix     uu(1, m, jlb, jub); uu.initialize();
  dmatrix     vv(1, m, jlb, jub); vv.initialize();
  dmatrix     DD(1, m, jlb, jub); DD.initialize();
  dmatrix     dmTotMort(1, m, jlb, jub); dmTotMort.initialize();
  dmatrix     dmTotalPredCaps(1, m, jlb, jub); dmTotalPredCaps.initialize();
  dmatrix     dmTotalObsCaps(1, m, jlb, jub); dmTotalObsCaps.initialize();

  dmatrix distances(0,m,0,n);
  dvector bins = displacement_bins(*this, distances);
  dmatrix  dmDisplacement(0, max_month, 0, bins.indexmax());
  dmDisplacement.initialize();
  dmatrix xDisplace(0,max_month,-(m+1),m+1); xDisplace.initialize();
  dmatrix yDisplace(0,max_month,-(n+1),n+1); yDisplace.initialize();

  dmatrix  dmCumulativeTotalPredCaps(1, m, jlb, jub);
           dmCumulativeTotalPredCaps.initialize();
  dmatrix  dmCumulativeTotalObsCaps(1, m, jlb, jub);
           dmCumulativeTotalObsCaps.initialize();
  dmatrix dmTotalCheck(1, 4, 0, max_month); dmTotalCheck.initialize();

  dmatrix  deaths(1, m, jlb, jub);
  dmatrix  cum_deaths(1, m, jlb, jub); cum_deaths.initialize();

  dvector        dvFleetCaps(0, nfleet);
  dvector dvTotalReturns(0, max_month); dvTotalReturns.initialize();
  dvector dvTotalTags(0, max_month); dvTotalTags.initialize();
  dvector dvRegionCaps; //(0, ngrid); dvRegionCaps.initialize();

  ivector ivEffortOccured(1, nfleet); ivEffortOccured.initialize();
  //ivector ivCohortNumber(0, nrelease); ivCohortNumber.initialize();

  d3_array d3aEffort = make_d3_array(0, nfleet, 1, m, jlb, jub);
  d3aEffort.initialize();
  d3_array d3aFishMort = make_d3_array(0, nfleet, 1, m, jlb, jub);
  d3aFishMort.initialize();
  HERE
#ifdef __BCG_SOLVER__
  linbcg bcg(*this);
  bcg.set_tol(1e-8);
  clogf << "\nUsing biconjugate gratient solver" << endl;
  clogf << "tol = " << bcg.get_tol() << endl;
#else
  interflag.setsaveinter(0);
  Upwind_coff_t coff(*this, m_ipar[6]);
  clogf << "\nUsing adi solver" << endl;
#endif

  int ncolumns = 0;
  int max_cohorts = 0;

  //int pool_tags = param.m_ipar[13];
  if (pool_tags == 0)
  {
    ncolumns = nrelease;
    max_cohorts = nrelease;
  }
  else if (pool_tags == 1)
  {
    ncolumns = nfleet;
    max_cohorts = 1;
  }
  else if (pool_tags == 2)
  {
    collapse_cohorts(recaps, nrec);
    ncolumns = tr[nrelease].cohort; //param.get_tr_cohort(param.get_nrelease());
    max_cohorts = tr[nrelease].cohort;
  }
  TTRACE(ncolumns,max_cohorts)

  d3_array d3aRelease = make_d3_array(0, ncolumns, 1, m, jlb, jub);
  d3aRelease.initialize();

  dmatrix dmTac(0,max_month,0,ncolumns);
  dmTac.initialize();

  //ivector cohort_released(0,max_cohorts); cohort_released.initialize();
  ivector cohort_alive(0,max_cohorts); cohort_alive.initialize();
  year_month_vector release_date(0,max_cohorts);

  //int numCurrentSeason = 0,
  int cMonth = 1;
  //                  rn = 0;   //release number
               //ncohort = 0;
  ofstream st;
  if (m_ipar67)
  {
    st.open("savdtags.dat");
    if (!st)
    {
      clogf << "Unable to open file savdtags.dat" << endl;
      return;
    }
    st << "#\n";
    st << "# tags at large\n";
    st << "#\n";
  }//ifm_ipar67

  ofstream capfile("simulate.tag");
  if ( !capfile )
  {
    cerr << "Cannot open file simulate.tag";
    exit(1);
  }
  capfile << "#\n";
  capfile << "# simulated tag recaptures\n";
  capfile << "# natural mortality = " << get_mort() << endl;
  capfile << "# catchability coefficients (by fleet) = " << get_q() << endl;
  capfile << "# random number seed = " << m_ipar[40] << endl;
  capfile << "# effort standard deviation = " << m_ipar[39]/100. << endl;
  capfile << "# \n";
  capfile << "#         positions: release      recapture\n";
  capfile << "#   fleet year month cohort long   lat      tags" << endl;


  double tcaps = 0.0;

  dvTotalTags[0] = tr[1].tags;
  dmTotalCheck[1][0] = tr[1].tags;
  dmTotalCheck[2][0] = tr[1].tags;
  dmTotalCheck[3][0] = tr[1].tags;
  dmTotalCheck[4][0] = tr[1].tags;

  int elapsed_month = -1;

   /*
  int ncohort = nrelease;
  int c1 = 1;
  int c2 = nrelease;
  int ac1 = 1;
  int ac2 = nrelease;

  if (pool_tags == 1)
  {
    c2 = 1;
    ac2 = 1;
  }
  else if (pool_tags == 2)
  {
    last_rn = 0;
    c1 = 1;
    c2 = nrelease;
  }
  TTRACE(c1,c2)
  */
  adstring status;
  int numCurrentSeason = 0;
  int rn = 0;
  int last_rn = 0;
  year_month final_date = tr[nrelease].date + nmonth;
  for (year_month date = start_date; date <= final_date; date++)
  {
    elapsed_month ++;
   // int numMonthSeason = get_season(date);
    cout << "Month" << setw(3) << elapsed_month << ", " << date << endl;
    
     uvs_comp(dmU, dmV, dmSigma, date);

     //this is for sending right graphics to seasonal wind
     int mo = date.get_month_value();
     if( m_ipar[159] )
     {
      if(( mo >= 1 && mo <= 4) ||  ( mo >= 10 && mo <= 12))  
        {
          jni->drawSquareRegion(wstress_ne, swc_plot);
        }
      if( mo >= 5 && mo <= 9 )
        {
           jni->drawSquareRegion(wstress_sw, swc_plot);
        } 
     } //if( m_ipar[159] )
    
      for (int i = 1; i <= m; i++)
      {
        int j1 = jlb(i);
        int j2 = jub(i);
        for (int j = j1; j <= j2; j++)
        {
          if (gridmap(i,j) > 0)
          {
            uu(i,j) = dmU(i,j);
            vv(i,j) = dmV(i,j);
            DD(i,j) = dmSigma(i,j);
          }
          else
          {
            uu(i,j) = 0.0;
            vv(i,j) = 0.0;
            DD(i,j) = 0.0;
          }
       } // j
     }  // i


    #ifdef __BCG_SOLVER__
      bcg.comp(*this, dmU, dmV, dmSigma);
    #else
      coff.comp(*this, date, isp, dmU, dmV, dmSigma);
    #endif

    if (m_ipar16)
    {
      cerr << "Invalid switch 16 = " << m_ipar16 << endl;
      exit(1);
    }
    if(m_ipar[45] == 1)
    {
      year_month effort_date(1,date.get_month_value());
      TTRACE(date,effort_date)
      get_effort_array(*this, global_irfr, d3aEffort, effort_date, ivEffortOccured);
    }
    else
      read_effort(*this, d3aEffort, date, ivEffortOccured);

    fish_mort_comp(d3aFishMort, d3aEffort, date);//mort=q*effort
    total_mort_comp(d3aFishMort, dmTotMort);
  #ifdef __BCG_SOLVER__
    bcg.b_adjust(dmTotMort);
  #else
    coff.b_adjust(*this, dmTotMort);
  #endif

    while( (rn <  nrelease) && (date == tr[rn + 1].date) )
    {
      rn++;
      int cohort = 0;
      if (pool_tags == 0)
        cohort = rn;
      else if (pool_tags ==1)
        cohort = 1;
      else if (pool_tags ==2)
        cohort = tr[rn].cohort;
      TTRACE(rn,cohort)
      int i = tr[rn].i;
      int j = tr[rn].j;

      d3aRelease[cohort][i][j] += tr[rn].tags;
      if (pool_tags == 2)
        cohort_alive(cohort) = nmonth+1;
      else
        cohort_alive(cohort) = max_month;

      release_date[cohort] = date;
      clogf << date << ": tag release #" << rn << " with "
            << tr[rn].tags << " tags added to cohort " << cohort << endl;
    }//while
    //ASSERT((rn <= nrelease))

    // place summ of all cohort slices into cohort 0
    for (int ii = 1; ii <= m; ii++)
    {
      int j1 = jlb(ii);
      int jn = jub(ii);
      for (int j=j1; j <= jn;j++)
      {
        double hold = 0.0;
        for (int cohort = 1; cohort <= max_cohorts; cohort++)
        {
          if (cohort_alive(cohort)>0)
            hold += d3aRelease[cohort][ii][j];
        }
        d3aRelease[0][ii][j] = hold;
      } //for j
    }//for ii

    obs_recap_comp(dmTotalObsCaps, date, 0, recaps,
                   nrec, *this, ivEffortOccured);

    dmCumulativeTotalObsCaps += dmTotalObsCaps;


    if (graphics_on)
    {
      HERE
      char buf[80];
      ostrstream ss(buf,80);
      ss << "Fit p" << setw(2) << setfill('0') << hex << m_ipar[8]
         << ": Month " << setw(3) << setfill(' ') << dec << elapsed_month
         << ", " << date
         << ends;
      //adstring status(buf);
      status = adstring(buf);
      jni->drawStatusBar(status);

      HERE
      char jbuf[80];
      ostrstream jss(jbuf,80);
    #ifdef unix
      jss << "jpeg/"
    #else
      jss << "jpeg\\"
    #endif
          << "td" << setw(2) << setfill('0') << hex << m_ipar[8] << dec
                  << setw(2) << setfill('0') << date.get_year()
                  << setw(2) << setfill('0') << date.get_month_value()
                  << ".png" << ends;
 
      jni->drawSquareRegion(d3aRelease[0], td_plot);
      jni->save_png(jbuf,640,480,td_plot);
      TRACE(jbuf)

      //dmatrix dmTotalFishMort = slice_sum(d3aFishMort);
      //jni->drawSquareRegion(1.0-exp(-1.0*dmTotalFishMort), hr_plot);

      jni->drawTriangleRegion(dmTotalObsCaps, dmTotalPredCaps, op_plot);
      HERE

      jni->drawTriangleRegion(dmCumulativeTotalObsCaps, dmCumulativeTotalPredCaps, cop_plot);
      HERE

      //int s = numCurrentSeason;
      //jni->drawArrowsMovementRegion(100.0*get_seasonal_u(s), 100.0*get_seasonal_v(s), rm_plot);
      //jni->drawCirclesMovementRegion(sqrt(get_seasonal_sigma(s)/M_PI), rm_plot);

      dvector pred_hist(1,nfleet+1);
      for (int fl = 1; fl <= nfleet; fl++)
      {
        pred_hist(fl+1) = dvFleetCaps(fl);
      }
      pred_hist(1) = sum(dvFleetCaps);
      //jni->drawLinePlotPoints(pred_hist, 0, 0, 2);

      history(2) = sum(dmTotalPredCaps);
      jni->drawLinePlot(history, history_plot);
      history(1) = sum(dmTotalObsCaps); // yes this seems out of step
      clogf << "hist: " << elapsed_month << " " << date << history << endl;
      //TTRACE(date,(history(1)-history(2)))

      HERE
      jni->drawSquareRegion(uu, u_plot);
      jni->drawSquareRegion(vv, v_plot);
      jni->drawSquareRegion(DD, dif_plot);
      jni->drawSquareRegion(DD, rm_plot);
      jni->drawArrows(uu,vv,45,45,rm_plot);  

      HERE
      if( m_ipar[155])
          jni->drawSquareRegion(depth, z_plot);
      if( m_ipar[156] )
            jni->drawSquareRegion(numfad, fad_plot);
      if( m_ipar[157] )            
          jni->drawSquareRegion(elevation, dz_plot);
      if( m_ipar[158] )
         jni->drawSquareRegion(distance, dst_plot);
     
        // see main loop for these
        // jni->drawSquareRegion(wstress_ne, swc_plot);
        // jni->drawSquareRegion(wstress_sw, swc_plot);
     
      if( m_ipar[160] ) 
        jni->drawSquareRegion(wstress_curl,wcurl_plot);
      
      HERE
      if ( !(jni->paintAll(500)) )
        exit(0);
    }//if (graphics_on)
    TRACE(graphics_on)

    for (int cohort = 1; cohort <= max_cohorts; cohort++)
    {
      TTRACE(cohort,cohort_alive(cohort))
      if (cohort_alive(cohort) > 0)
      {
        int i_unused = 0;
        float f_unused = 0.0;
        capcomp(date, cohort, *this, d3aRelease[cohort],
                capfile, tcaps, dvRegionCaps, dmTotMort, d3aFishMort,
                ivEffortOccured, dvFleetCaps, f_unused, i_unused,
                dmTotalPredCaps);
        dmCumulativeTotalPredCaps += dmTotalPredCaps;
        double pcaps = sum(dvFleetCaps);
        int mtac = 0;
        if (pool_tags == 1)
          mtac = elapsed_month;
        else
          mtac = date - release_date[cohort]; //tr[cohort].date;

        //ASSERT((mtac >= 0))
        if (pool_tags == 0)
          dmTac(mtac, cohort) += pcaps;
        else if (pool_tags == 1)
          dmTac(mtac) = dvFleetCaps;
        else if (pool_tags == 2)
          dmTac(mtac, cohort) += pcaps;

        dmTac(mtac, 0) += pcaps;

        dvTotalReturns(cMonth - 1) += pcaps;

      // solve the PDE
      #ifdef __BCG_SOLVER__
        bcg.solver(d3aRelease[cohort], m_ipar6);
      #else
        coff.adi(d3aRelease[cohort], isp);
      #endif

        displacement_frequency(cohort, d3aRelease[cohort],
                        dmDisplacement(mtac), xDisplace(mtac), yDisplace(mtac),
                        distances, bins, *this);

        deaths = elem_prod(d3aRelease[cohort],(1-exp(-dmTotMort)));
        double fished = sum(deaths - d3aRelease[cohort]*(1-exp(-mort)));
        TRACE(fished)
        cum_deaths += deaths;
        TTRACE(min(cum_deaths),max(cum_deaths))
        TRACE(sum(cum_deaths))

        cohort_alive(cohort) -- ;
      } // if (cohort_alive(cohort) > 0)

    }//forcohort

    if (m_ipar67)
      save_tags(*this, d3aRelease[0], date, st);

    total_check_comp(dvTotalTags, dmTotalCheck, cMonth-1,
                     d3aRelease[0], *this, dmTotMort);

    cMonth++;
  }//for date
  cout << "Finished date loop!" << endl;
  HERE
  if (graphics_on)
  {
    status += adstring(" ... Finished");
    jni->drawStatusBar(status);
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
  write_displacement_files(dmDisplacement, xDisplace, yDisplace, bins,
                           m_ipar[8],'p');

  //-----------------------------------------------------------
  clogf << "\nWarning! The first and last column below will only agree if there\n"
                 "is only 1 tag release cohort, fishing effort is uniform over all regions,\n"
                 "and the effort standard deviation (m_ipar[49]) is 0. Check these before you\n"
                 "try to fix something.\n";

  clogf << "\nMonthly totals for tag cohort 0\n";
  clogf << setw(17) << "grid sum:"
        << setw(15) << "implicit"
        << setw(14) << "explicit"
        << setw(14) << "average"
        << setw(14) << "exact\n";

  for (int month = 0; month <= max_month; month++)
  {
     clogf << setw(3)<< month << setw(14) << setprecision(5)
           << dvTotalTags[month] << ":";
     for (int ktc = 1; ktc <= 4; ktc++)
       clogf << setw(14) << setprecision(5) << dmTotalCheck[ktc][month];
     clogf << "\n";
  }//formonth

  //-----------------------------------------------------------

  clogf << "\nTotal recaptures: " << setprecision(5) << tcaps << "\n";
  //----------------------------------------------------

  //adstring prn_name = pathname.getRoot() + ".atc";
  char prn_name[80];
  ostrstream ss(prn_name,80);
  ss << "fit" << setw(2) << setfill('0') << hex << m_ipar[8]
	  << "_P" << pool_tags << ".pta" << '\0';
  TRACE(prn_name)
  prnstream prn(prn_name);
  //if (!prn)
  //{
  //  cerr << "Error opening prn_name" << endl;
  //  exit(1);
  //}
  cout << "predicted tag attrition curve" << PENDL;
  prn << "predicted tag attrition curve" << PENDL;
  prn << "months at" << "aggregate";
  int prn_cols = ncolumns; //0;

  //if (m_ipar13)
  if (pool_tags == 0)
  {
    //prn_cols = nrelease;
    prn << "disaggregated release group" << PENDL;
  }
  if (pool_tags == 1)
  {
    //prn_cols = nfleet;
    prn << "recapture fleet" << PENDL;
  }
  else if (pool_tags == 2)
  {
    //prn_cols = nrelease;
    prn << "release group aggregated by release date" << PENDL;
  }
  prn << "liberty" << "recaptures";

//  for (int cohort = 0; cohort <= prn_cols; cohort++)
//    if (pool_tags == 1)
//      prn << cohort;
//    else
//      prn << ivCohortNumber(cohort);
  for (int column = 0; column <= ncolumns; column++)
  {
    if (column == 0)
      prn << "All";
    else
    {
      if (pool_tags == 0)
        prn << get_tr_cohort(column);

      else if (pool_tags == 1)
        prn << (char*)fleet_names(column);

      else if (pool_tags == 2)
        prn << column;

    }
  }  prn << PENDL;

  for (int month = 0; month <= max_month; month++)
  {
    prn << month << dvTotalReturns(month);
    clogf << month <<" "<< dvTotalReturns(month);
    for (int cohort = 0; cohort <= prn_cols; cohort++)
    {
      prn << dmTac(month, cohort);
      clogf <<"," << dmTac(month, cohort);
    }
    prn << PENDL;
    clogf << "\n";
  }//formonth
  prn.close();

  cout << "Finished final calculations." << endl;

  if (graphics_on)
  {
    // wait untile the graphics system is closed
    while ( (jni->paintAll()) ) {}
    exit(0);
  }

}//End par_t_nn::simulate(JNIEnv*,jobject,recaptype_vector&,int)
