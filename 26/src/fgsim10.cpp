//$Id: fgsim10.cpp 3163 2014-04-26 01:58:02Z eunjung $
#include <time.h>
#include <sstream>
#include <ostream>
#include "prnstrem.h"
#include "par_t_reg.h"
#include "fish_rec.h"
#include "trace.h"

//extern int saveinterflag;

double min(const dmatrix& t);
extern indexed_regional_fishery_record global_irfr;
#include <jnigraphics2.h>
#ifdef __BCG_SOLVER__
  #include "linbcg.h"
#else
  #include "coff_t.h"
  //#include "intersav.h"
  //extern setsaveinterflag interflag;
  //extern intersavetype *isp;
#endif

 
void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr,
       d3_array& effort, year_month& date,
       ivector& effort_occured);

void set_effort_to_recaps_and_mean(d3_array& effort, year_month& date, 
    indexed_regional_fishery_record& irfr, 
    const recaptype_vector& recaps, int recap_recsi, ivector& effort_occured, const int ipar52);


dvar_matrix slice_sum(const dvar3_array& t);

void capcomp(year_month& date, int cc, par_t& param, dvar_matrix& tags,
             ofstream& capfile, dvariable& tcaps, dvar_vector& region_caps,
             dvar_matrix& total_mort, dvar3_array& fish_mort, ivector& effort_occured,
             dvar_vector& pred_tac, float f_unused, int i_unused, dvar_matrix& total_pred_caps);

void save_tags(par_t& param, dvar_matrix& releases, year_month& date,
               ofstream& savefile);

//void total_check_comp(dvector& total_tags, dmatrix& total_check,
//                      int month, dvar_matrix& release, par_t& param,
//                      dvar_matrix& tot_mort);

dvector displacement_bins(par_t& param, dmatrix& distances);
void displacement_frequency(const int cohort, const dvar_matrix& tags, 
                        dvector& displacements, dvector& xdisp, dvector& ydisp, 
                        const dmatrix& distances, const dvector& bins, 
                        par_t& param);
void write_displacement_files(const dmatrix& displacement, 
                         dmatrix& xdisp, dmatrix& ydisp, 
                         const dvector& bins,
                         const int fit_number, const char op);

d3_array make_d3_array(int sl,int sh, int nrl, int nrh,
                       const ivector& ncl, const ivector& nch);

dvar3_array make_dvar3_array(int sl,int sh, int nrl, int nrh,
                       const ivector& ncl, const ivector& nch);
void check_release(const dmatrix& dmRelease, const int row);

double tag_scaling_factor = 1.0;

void write_xyz(const dvar_matrix& m, const par_t_reg* p, 
               const adstring id, const int fit, const year_month date);
void write_bitmap(jnigraphics2* jni, const adstring id, const int fit, const year_month date, const int panel);
void write_bitmap(jnigraphics2* jni, const adstring id, const int fit, const int panel);

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

/** Does the ADRM simulation.
\param recaps vector of tag recaptures.
\param nrec integer containing number of tag recapture records.
\param path adstring containing path to current directory; usually "./".
\param file adstring containing file name root for the analysis, e.g. "skj16".
\param _env Not used
\param _jobj Not used
\todo Get rid of unused arguments to simulate()
*/
void par_t_reg::simulate(JNIEnv* _env, jobject _jobj,
                         recaptype_vector& recaps, int nrec,
                         const adstring& path, const adstring& file)
{
  cout << __FILE__ << ": par_t_reg::simulate" << endl;
  int pool_tags = m_ipar[13],   //Pool tags
      m_ipar16 = m_ipar[16],
      m_ipar67 = m_ipar[67],
      m_ipar68 = m_ipar[68];  // graphics off

  int graphics_on = m_ipar68;

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
#endif

  int max_month = tr[nrelease].date - tr[1].date + nmonth + 1;
  if (max_month < 60)
    max_month = 60;

  jnigraphics2* jni = NULL;

  //const int ntabs = m_ipar(85)==1?6:5;
  int ntabs = 5;
  if (m_ipar(85)==1 || m_ipar(85)==2)
     ntabs ++;
  if (m_ipar(53) > 0)
     ntabs ++;

  ivector tab_list(1,ntabs);
  int hr_plot = 0;
  int hr1_plot = 0;
  int hr0_plot = 0;
  int td_plot = 0;
  int rm_plot = 0;
  int op_plot = 0;
  int cop_plot = 0;
  int history_plot = 0;
  int dif_plot = 0;
  int adv_plot = 0;
  int d_plot = 0;
  int cd_plot = 0;
  int fad_plot = 0;
  int mp_plot = 0;
  int gx_plot =0;
  int gy_plot =0;
  int td2_plot = 0;


  if (graphics_on)
  {
    jni = new jnigraphics2();
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
    int deaths_tab = tab_list(5);

    int tab_counter = 5;
    int fadmap_tab = 0;
    int missing_effort_tab = 0;
    if (m_ipar(83)==1)
    {
      tab_counter ++;
      fadmap_tab = tab_list(tab_counter);
    }
    if (m_ipar(53) > 0)
    {
      tab_counter ++;
      missing_effort_tab = tab_list(tab_counter);
    }
    
    jni->setTabTitle("Monthly",monthly_tab);
    ivector mt_layout(1, 4);
    jni->addGridLayout(2, 2, mt_layout, monthly_tab);
    hr_plot = mt_layout(1);
    td_plot = mt_layout(2);
    rm_plot = mt_layout(3);
    op_plot = mt_layout(4);

    jni->addStatusBar();

    squareregiondef scm(m, n);
    scm.region.dx = int(deltax);
    scm.region.dy = int(deltay);
    scm.region.title = (char*)"Harvest Ratio";
    scm.region.setGridMap(gridmap);
    if (file_exists(coastline_file))
      scm.region.cstfile = coastline_file;
    scm.legend.min = 0.0;
    scm.legend.max = 1.0;
    scm.legend.increment = 0.2;
    scm.legend.decimals = 1;
    jni->addSquareRegion(scm, hr_plot);

    scm.region.title = (char*)"Tag Density";
    scm.region.setGridMap(gridmap);
    scm.legend.min = 1.0;
    scm.legend.max = 1000.0; //1024.0;
    scm.legend.type = 1;
    scm.legend.increment = 2;
    scm.legend.decimals = 0;
    jni->addSquareRegion(scm, td_plot);

    movementregiondef mrd(m, n);
    mrd.region.dx = int(deltax);
    mrd.region.dy = int(deltay);
    mrd.region.title = (char*)"Movement Pattern";
    mrd.setGridMap(gridmap);
    mrd.region.cstfile = coastline_file;
    jni->addMovementRegion(mrd, rm_plot);

    triangleregiondef tcm(m, n);
    tcm.region.dx = int(deltax);
    tcm.region.dy = int(deltay);
    tcm.region.title = (char*)"Observed/Predicted Returns";
    tcm.region.setGridMap(gridmap);
    if (file_exists(coastline_file))
      tcm.region.cstfile = coastline_file;
    tcm.legend.min = 1.0;
    tcm.legend.max = 256;
    tcm.legend.type = 1;
    tcm.legend.increment = 2;
    jni->addTriangleRegion(tcm, op_plot);

    jni->setTabTitle("Cumulative Returns",cumulative_tab);
    ivector ct_layout(1, 1);
    jni->addGridLayout(1, 1, ct_layout, cumulative_tab);
    cop_plot = ct_layout(1);
    tcm.region.title = (char*)"Observed/Predicted Returns";
    tcm.legend.min = 1.0;
    tcm.legend.max = 512;
    tcm.legend.type = 1;
    tcm.legend.increment = 2;
    jni->addTriangleRegion(tcm, cop_plot);

    jni->setTabTitle("History", history_tab);
    ivector ht_layout(1,1);
    jni->addGridLayout(1, 1, ht_layout, history_tab);
    history_plot = ht_layout(1);
    lineplotdefs hpd;//(2);
    hpd.linesTotal = 4;
    hpd.xaxisTitle = (char*)"Months After First Tag Release";
    hpd.xaxisMax = ((max_month/5)+1)*5;
    hpd.xaxisIncrement = 5;
    hpd.yaxisTitle = (char*)"Tag Returns";
    hpd.yaxisMax = 600.0;
    hpd.yaxisIncrement = 100.0;
    hpd.setColor(0,0,255,0);
    hpd.setColor(0,255,0,1);
    hpd.setColor(0,255,255,2);
    hpd.setColor(255,0,0,3);
    jni->addLineplot(hpd, history_plot);

    jni->setTabTitle("Movement Fields", movement_tab);
    /*
    ivector mv_layout(1,2);
    if (m > n)
      jni->addGridLayout(2, 1, mv_layout, movement_tab);
    else
      jni->addGridLayout(1, 2, mv_layout, movement_tab);
    //int* mvlp = jni->addGridLayout(2, 1, movement_tab);
    dif_plot = mv_layout(1);// = mvlp[0];
    scm.region.title = (char*)"Diffusion Field";
    scm.region.setGridMap(gridmap);
    scm.legend.min = 1.0;
    scm.legend.max = 262144.0;
    scm.legend.type = 1;
    scm.legend.increment = 8.0;
    scm.legend.decimals = 0;
    jni->addSquareRegion(scm, dif_plot);
    */
    ivector mv_layout(1,1);
    jni->addGridLayout(1, 1, mv_layout, movement_tab);
    dif_plot = mv_layout(1);
    scm.region.title = (char*)"Movement Field";
    scm.region.setGridMap(gridmap);
    scm.legend.min = 1;
    if (m_ipar(83) == 1)
      scm.legend.max = 20000.0;
    else
      scm.legend.max = 262144.0; // 2^18
    
    scm.legend.type = 1;
    scm.legend.increment = 4.0;
    scm.legend.decimals = 0;
    jni->addSquareRegion(scm, dif_plot);
    
    //adv_plot = mv_layout(2);// = mvlp[1];
    advectionregiondef adv(m,n);
    adv.region.title = (char*)"Advection Field";
    adv.region.setGridMap(gridmap);
    jni->addAdvectionRegion(adv, adv_plot);
    
    jni->setTabTitle("Lifetime Displacements", deaths_tab);
    ivector dt_layout(1,2);
    if (m > n)
      jni->addGridLayout(2, 1, dt_layout, deaths_tab);
    else
      jni->addGridLayout(1, 2, dt_layout, deaths_tab);

    d_plot = dt_layout(1);
    scm.region.title = (char*)"Monthly Mortalities";
    scm.region.setGridMap(gridmap);
    scm.legend.min = 1.0;
    scm.legend.max = 128.0;
    scm.legend.type = 1;
    scm.legend.increment = 2.0;
    scm.legend.decimals = 0;
    jni->addSquareRegion(scm, d_plot);

    cd_plot = dt_layout(2);
    scm.region.title = (char*)"Cumulative Mortalities";
    scm.region.setGridMap(gridmap);
    scm.legend.min = 0.1;
    scm.legend.max = 100.0;
    scm.legend.type = 0;
    scm.legend.increment = 20;
    scm.legend.decimals = 0;
    jni->addSquareRegion(scm, cd_plot);

    
    if (m_ipar(83) == 1)
    {
      jni->setTabTitle("FADmap", fadmap_tab);    
      //ivector fm_layout(1,2);
      //jni->addGridLayout(1,1,fm_layout,fadmap_tab);
  

      //if (m > n)
      //   jni->addGridLayout(2, 1, fm_layout, fadmap_tab);
      //else
      //   jni->addGridLayout(1, 2, fm_layout, fadmap_tab);
      ivector fm_layout(1,4);
      jni->addGridLayout(2, 2, fm_layout, fadmap_tab);

      fad_plot = fm_layout(1);
      scm.region.title = (char*)"FAD density";
      scm.region.setGridMap(gridmap);
      scm.legend.min = 1;
      scm.legend.max = 60.0;
      scm.legend.type = 1;
      scm.legend.increment = 1;
      scm.legend.decimals = 0;
      jni->addSquareRegion(scm,fad_plot);         
  
      //mp_plot = fm_layout(2);
      //mrd.region.title = (char*)"Movement Pattern";
      //mrd.setGridMap(gridmap);
      //mrd.region.cstfile = coastline_file;
      //jni->addMovementRegion (mrd, mp_plot);
      
      
      gx_plot = fm_layout(2);
      scm.region.title = (char*)"gx of FAD density";
      scm.region.setGridMap(gridmap);
      scm.legend.min = -1.8;
      scm.legend.max = 0.6;
      scm.legend.increment = 0.2;
      scm.legend.decimals = 1;
      scm.legend.type = 0;
      jni->addSquareRegion(scm,gx_plot);         
      
      gy_plot = fm_layout(3);
      scm.region.title = (char*)"gy of FAD density";
      scm.region.setGridMap(gridmap);
      scm.legend.min = -1.8;
      scm.legend.max = 0.6;
      scm.legend.increment = 0.2;
      scm.legend.decimals = 1;
      scm.legend.type = 0;
      jni->addSquareRegion(scm,gy_plot);         
   
      td2_plot = fm_layout(4);
      scm.region.title = (char*)"Tag Density";
      scm.region.setGridMap(gridmap);
      scm.legend.min = 1.0;
      scm.legend.max = 1000.0; //1024.0;
      scm.legend.type = 1;
      scm.legend.increment = 2;
      scm.legend.decimals = 0;
      jni->addSquareRegion(scm, td2_plot);
      
    } 
    
    if (m_ipar(53) > 0)
    {
      jni->setTabTitle("Missing Effort Comparision", missing_effort_tab);
      ivector me_layout(1,2);
      if (m > n)
        jni->addGridLayout(2,1,me_layout,missing_effort_tab);
      else
        jni->addGridLayout(1,2,me_layout,missing_effort_tab);

      scm.legend.min = 0.0;
      scm.legend.max = 1.0;
      scm.legend.increment = 0.2;
      scm.legend.decimals = 1;

      hr1_plot = me_layout(1);
      scm.region.title = (char*)"Harvest Ratio E > 0";
      jni->addSquareRegion(scm, hr1_plot);

      hr0_plot = me_layout(2);
      scm.region.title = (char*)"Harvest Ratio E = 0";
      jni->addSquareRegion(scm, hr0_plot);
    }
    

    jni->layoutAll();
  }

  dvector history(1,4); history.initialize(); // for line plots

  dvar_matrix           dmU(0, m + 1, jlb1, jub1); dmU.initialize();
  dvar_matrix           dmV(0, m + 1, jlb1, jub1); dmV.initialize();
  dvar_matrix       dmSigma(0, m + 1, jlb1, jub1); dmSigma.initialize();
  dmatrix       uu(1, m, jlb, jub); uu.initialize();
  dmatrix       vv(1, m, jlb, jub); vv.initialize();
  dmatrix       DD(1, m, jlb, jub); DD.initialize();
  dvar_matrix           dmTotMort(1, m, jlb, jub); dmTotMort.initialize();
  dvar_matrix     dmTotalPredCaps(1, m, jlb, jub); dmTotalPredCaps.initialize();
  dmatrix      dmTotalObsCaps(1, m, jlb, jub); dmTotalObsCaps.initialize();

  dmatrix distances(0,m,0,n);
  dvector bins = displacement_bins(*this, distances);
  dmatrix  dmDisplacement(0, max_month, 0, bins.indexmax());
  dmDisplacement.initialize();
  dmatrix xDisplace(0,max_month,-(m+1),m+1); xDisplace.initialize();
  dmatrix yDisplace(0,max_month,-(n+1),n+1); yDisplace.initialize();

  dvar_matrix  dmCumulativeTotalPredCaps(1, m, jlb, jub);
               dmCumulativeTotalPredCaps.initialize();
  dmatrix  dmCumulativeTotalObsCaps(1, m, jlb, jub);
           dmCumulativeTotalObsCaps.initialize();
  dmatrix dmTotalCheck(1, 4, 0, max_month); dmTotalCheck.initialize();

  dmatrix  deaths(1, m, jlb, jub);
  dmatrix  cum_deaths(1, m, jlb, jub); cum_deaths.initialize();

  dvar_vector        dvFleetCaps(0, nfleet);
  dvar_vector dvTotalReturns(0, max_month); dvTotalReturns.initialize();
  dvector dvTotalTags(0, max_month); dvTotalTags.initialize();
  dvar_vector dvRegionCaps(0, ngrid); dvRegionCaps.initialize();
  dvector dvObsRegionCaps(0, ngrid); dvObsRegionCaps.initialize();

  ivector ivEffortOccured(1, nfleet); ivEffortOccured.initialize();
  //ivector ivCohortNumber(0, nrelease); ivCohortNumber.initialize();

  /* Observed tag returns for which there IS reported effort).*/
  d3_array Recaps1 = make_d3_array(0, nfleet, 1, m, jlb, jub);
  Recaps1.initialize();
  /* Observed tag returns for which there is NO reported effort).*/
  d3_array Recaps0 = make_d3_array(0, nfleet, 1, m, jlb, jub);
  Recaps0.initialize();

  d3_array d3aEffort = make_d3_array(0, nfleet, 1, m, jlb, jub);
  d3aEffort.initialize();
  dvar3_array FishMort1 = make_dvar3_array(0, nfleet, 1, m, jlb, jub);
  //dvar3_array FishMort1(0, nfleet, 1, m, jlb, jub);
  FishMort1.initialize();
  dvar3_array FishMort0 = make_dvar3_array(0, nfleet, 1, m, jlb, jub);
  FishMort0.initialize();
  HERE
#ifdef __BCG_SOLVER__
  linbcg bcg(*this);
  bcg.set_tol(1e-8);
  clogf << "\nUsing biconjugate gratient solver" << endl;
  clogf << "tol = " << bcg.get_tol() << endl;
#else
  //interflag.setsaveinter(0);
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

  dvar3_array d3aRelease = make_dvar3_array(0, ncolumns, 1, m, jlb, jub);
  d3aRelease.initialize();

  dvar_matrix dmTac(0,max_month,0,ncolumns);
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

  time_t capfile_time;
  time(&capfile_time);
  capfile << "#\n";
  capfile << "# simulated tag recaptures\n";
  capfile << "# created " << ctime(&capfile_time);// << endl;
  capfile << "# natural mortality = " << get_mort() << endl;
  capfile << "# catchability coefficients (by fleet) = " << get_q() << endl;
  capfile << "# random number seed = " << m_ipar[40] << endl;
  capfile << "# effort standard deviation = " << m_ipar[39]/100. << endl;
  capfile << "# \n";
  capfile << "#         positions: release      recapture\n";
  capfile << "#   fleet year month cohort long   lat      tags" << endl;


  dvariable tcaps = 0.0;

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

  int numCurrentSeason = 0;
  int seasonChanged = 0;
  int rn = 0;
  year_month final_date = tr[nrelease].date + nmonth;
  for (year_month date = start_date; date <= final_date; date++)
  {
    elapsed_month ++;
    int numMonthSeason = get_season(date);
    cout << "Month" << setw(3) << elapsed_month << ", " << date << ", season " << numMonthSeason << endl;
    //season change redo uvs
    if (numCurrentSeason != numMonthSeason || m_ipar[83]==1)
    {
      seasonChanged = 1;
      numCurrentSeason = numMonthSeason;
      uvs_comp(dmU, dmV, dmSigma, date);

      for (int i = 1; i <= m; i++)
      {
        int j1 = jlb(i);
        int j2 = jub(i);
        for (int j = j1; j <= j2; j++)
        {
          if  (gridmap(i,j) > 0)
          {
            uu(i,j) = value(dmU(i,j));
            vv(i,j) = value(dmV(i,j));
            DD(i,j) = value(dmSigma(i,j));
          }
          else
          {
            uu(i,j) = 0.0;
            vv(i,j) = 0.0;
            DD(i,j) = 0.0;
          }
          if (fabs(uu(i,j)) > 1e10)
            uu(i,j) = 0.0;
          if (fabs(vv(i,j)) > 1e10)
            vv(i,j) = 0.0;
          if (fabs(DD(i,j)) > 1e20)
            DD(i,j) = 0.0;
        }
      }

    #ifdef __BCG_SOLVER__
      bcg.comp(*this, dmU, dmV, dmSigma);
    #else
      coff.comp(*this, dmU, dmV, dmSigma);
    #endif
    }//ifnumMonthSeason
    else
      seasonChanged = 0;

    if (m_ipar16)
    {
      cerr << "Invalid switch 16 = " << m_ipar16 << endl;
      exit(1);
    }

    #undef USE_EFFORT_STREAM
    #ifdef USE_EFFORT_STREAM
    if(m_ipar[45] == 1)
    {
      year_month effort_date(1,date.get_month_value());
      get_effort_array(*this, global_irfr, d3aEffort, effort_date, ivEffortOccured);
    }
    else
    {
      read_effort(*this, d3aEffort, date, ivEffortOccured);
      if (m_ipar[53] == 1)
      {
         set_effort_to_recaps_and_mean(d3aEffort, date, global_irfr, 
           recaps, nrec, ivEffortOccured, m_ipar[52]);
      }
    } 
    #else

    //if(m_ipar[45] == 1)
    //  get_average_effort_array(*this, global_irfr, d3aEffort, date, ivEffortOccured);
    //else
      get_effort_array(*this, global_irfr, d3aEffort, date, ivEffortOccured);

    #endif //USE_EFFORT_STREAM

    //fish_mort_comp(FishMort1, d3aEffort, date);//mort=q*effort

    fish_mort_comp(FishMort1, FishMort0, d3aEffort, date, d3aRelease(0), Recaps0, global_irfr.get_mean_effort() );//mort=q*effort
    total_mort_comp(FishMort1, FishMort0, dmTotMort);
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
    /*
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
          {
            if (isnan[cohort][ii][j])
            {
               TRACE(cohort)
               TTRACE(ii,j)
               exit(1);
            }
            hold += d3aRelease[cohort][ii][j];
          }
        }
        d3aRelease[0][ii][j] = hold;
      } //for j
    }//for ii
    */
    d3aRelease(0).initialize();
    for (int cohort = 1; cohort <= max_cohorts; cohort++)
    {
      d3aRelease(0) +=  d3aRelease(cohort);
    }

    //obs_recap_comp(dmTotalObsCaps, date, 0, recaps,
    //               nrec, *this, ivEffortOccured);

    recaps.getreturns(Recaps1, Recaps0, date, 0);
    dmTotalObsCaps = Recaps1(0)+Recaps0(0);
    dmCumulativeTotalObsCaps += dmTotalObsCaps;
   

    if (graphics_on)
    {
      //char buf[80];
      //ostrstream ss(buf,80);
      std::ostringstream ss;
      ss << "Fit p" << setw(2) << setfill('0') << hex << m_ipar[8]
         << ": Month " << setw(3) << setfill(' ') << dec << elapsed_month
         << ", " << date
         << ", Season " << numCurrentSeason
         << ends;
      //adstring status(buf);
      adstring status(ss.str().c_str());
      jni->drawStatusBar(status);
      jni->drawSquareRegion(value(d3aRelease[0]), td_plot);
      write_bitmap(jni, "td", m_ipar(8),  date, td_plot);
      write_xyz(d3aRelease[0], this,  "td", m_ipar(8),  date);
      
      dvar_matrix dmTotalFishMort = slice_sum(FishMort1)+slice_sum(FishMort0);
      jni->drawSquareRegion(value(1.0-mfexp(-1.0*dmTotalFishMort)), hr_plot);
      jni->drawTriangleRegion(dmTotalObsCaps, value(dmTotalPredCaps), op_plot);
      write_bitmap(jni,"op", m_ipar(8),  date, op_plot);

      jni->drawTriangleRegion(dmCumulativeTotalObsCaps, value(dmCumulativeTotalPredCaps), cop_plot);

      if(m_ipar(83)==1)
      {
          jni->drawSquareRegion(gx_fadmap(elapsed_month+1), gx_plot);
          jni->drawSquareRegion(gy_fadmap(elapsed_month+1), gy_plot);
          jni->drawSquareRegion(fadmap(elapsed_month+1), fad_plot);
          jni->drawSquareRegion(value(d3aRelease[0]), td2_plot);

        dvector fadcircles(1,ngrid);
        for (int grid = 1; grid <= ngrid; grid++)
        {
          double sum = 0;
          int count = 0;
          for (int i = 1; i <= m; i++)
          {
            for (int j = 1; j <= n; j++)
            {
              if (gridmap(i,j) == grid)
              {
                sum += value(dmSigma(i,j));
                count++;
              }
            }
          }
          fadcircles(grid) = sum/count;   //average sigma
        }
          
        if (seasonChanged)
        {
          int s=numCurrentSeason;
          jni->drawArrowsMovementRegion(value(get_seasonal_u(s)),
                                        value(get_seasonal_v(s)), fad_plot);
        
        }
        //cout << fadcircles << endl;
        jni->drawCirclesMovementRegion(sqrt(fadcircles/M_PI), fad_plot);
        cout << "dmSigma" << dmSigma << endl;
        if (seasonChanged)
        {
          write_bitmap(jni,"fad", m_ipar(8),  date, fad_plot);
        }
      } // if(m_ipar(83)==1)

      if (m_ipar(53) > 0)
      {
        dmatrix TotalF1 = value(slice_sum(FishMort1));
        dmatrix TotalF0 = value(slice_sum(FishMort0));
        jni->drawSquareRegion(1.0-mfexp(-1.0*TotalF1), hr1_plot);
        jni->drawSquareRegion(1.0-mfexp(-1.0*TotalF0), hr0_plot);

      }
    
      if (seasonChanged)
      {
        int s = numCurrentSeason;
        jni->drawArrowsMovementRegion(value(get_seasonal_u(s)), 
                                      value(get_seasonal_v(s)), rm_plot);
        jni->drawCirclesMovementRegion(sqrt(value(get_seasonal_sigma(s))/M_PI), rm_plot);
        //write_bitmap(jni,"uvD", m_ipar(8),  date, rm_plot);

        jni->drawSquareRegion(DD, dif_plot);
        if (m_ipar(83)==1)
          jni->drawArrows(uu,vv,50,50,dif_plot);  
        else
          jni->drawArrows(uu,vv,5,5,dif_plot);  
        write_bitmap(jni,"uvD", m_ipar(8),  date, dif_plot);
      }
      dvector pred_hist(1,nfleet+1);
      for (int fl = 1; fl <= nfleet; fl++)
      {
        pred_hist(fl+1) = value(dvFleetCaps(fl));
      }
      pred_hist(1) = value(sum(dvFleetCaps));

      //history(2) = sum(dmTotalPredCaps);
      history(4) = value(sum(dmTotalPredCaps));
      jni->drawLinePlot(history, history_plot);
      //history(1) = sum(dmTotalObsCaps); // yes this seems out of step
      history(1) = sum(Recaps0(0))+sum(Recaps1(0));
      history(2) = sum(Recaps0(0)); 
      history(3) = sum(Recaps1(0)); 
      clogf << "hist: " << elapsed_month << " " << date << history << endl;

      jni->drawSquareRegion(deaths, d_plot);
      jni->drawSquareRegion(100.0*(cum_deaths/(max(cum_deaths)+1e-7)), cd_plot);

      if ( !(jni->paintAll(500)) )
        exit(0);
    }//if (graphics_on)

    for (int cohort = 1; cohort <= max_cohorts; cohort++)
    {
      if (cohort_alive(cohort) > 0)
      {
        int i_unused = 0;
        float f_unused = 0.0;
      #undef USE_EFFORTLESS_RECAPS
      #ifdef USE_EFFORTLESS_RECAPS
        d3_array tFishMort = FishMort1 + FishMort0;
        TTRACE(date,sum(tFishMort))
        TTRACE(sum(FishMort1),sum(FishMort0))
        capcomp(date, cohort, *this, d3aRelease[cohort],
                capfile, tcaps, dvRegionCaps, dmTotMort, tFishMort,
                ivEffortOccured, dvFleetCaps, f_unused, i_unused,
                dmTotalPredCaps);
      #else
        capcomp(date, cohort, *this, d3aRelease[cohort],
                capfile, tcaps, dvRegionCaps, dmTotMort, FishMort1,
                ivEffortOccured, dvFleetCaps, f_unused, i_unused,
                dmTotalPredCaps);
      #endif
        dmCumulativeTotalPredCaps += value(dmTotalPredCaps);
        dvariable pcaps = sum(dvFleetCaps);
        int mtac = 0;
        if (pool_tags == 1)
          mtac = elapsed_month;
        else
          mtac = date - release_date[cohort]; //tr[cohort].date;

        if (mtac > max_month)
          mtac = max_month;
        //ASSERT((mtac <= dmTac.rowmax()))
        //ASSERT((mtac >= 0))
        if (pool_tags == 0)
          dmTac(mtac, cohort) += pcaps;
        else if (pool_tags == 1)
          dmTac(mtac) = dvFleetCaps;
        else if (pool_tags == 2)
          dmTac(mtac, cohort) += pcaps;

        dmTac(mtac, 0) += pcaps;

        if ((cMonth-1) > dvTotalReturns.indexmax())
           cMonth =  dvTotalReturns.indexmax()+1;

        dvTotalReturns(cMonth - 1) += pcaps;
           
        displacement_frequency(cohort, d3aRelease[cohort],
                        dmDisplacement(mtac), xDisplace(mtac), yDisplace(mtac),
                        distances, bins, *this);

      // solve the PDE
      #ifdef __BCG_SOLVER__
        int m_ipar6 = m_ipar[6];
        bcg.solver(d3aRelease[cohort], m_ipar6);
      #else
        coff.adi(d3aRelease[cohort]); //isp);
      #endif

        deaths = value(elem_prod(d3aRelease[cohort],(1-mfexp(-dmTotMort))));
        cum_deaths += deaths;

        cohort_alive(cohort) -- ;
      } // if (cohort_alive(cohort) > 0)

    }//forcohort

    if (m_ipar67)
      save_tags(*this, d3aRelease[0], date, st);

    //total_check_comp(dvTotalTags, dmTotalCheck, cMonth-1,
    //                 d3aRelease[0], *this, dmTotMort);

    cMonth++;
  }//for date
  if (graphics_on)
  {
    jni->drawStatusBar(adstring(" Finished"));
  }
  cout << "Finished date loop!" << endl;
  clogf << "\nFinished date loop!" << endl;

  if (graphics_on)
  {
    //write_bitmap(jni,"final", m_ipar(8), final_date, cop_plot);
    write_bitmap(jni,"final", m_ipar(8), cop_plot);
    write_bitmap(jni,"history", m_ipar(8), history_plot);
  } // if (graphics_on)

  HERE
  write_displacement_files(dmDisplacement, xDisplace, yDisplace, bins,
                           m_ipar[8],'p');
 
  //-----------------------------------------------------------
  clogf <<
"\nWarning! The first and last column below will only agree if there\n"
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
  double tObsCaps = 0.0;
  for (int k = 1; k <= nrec; k++)
  {
    const int i = recaps[k].i;
    const int j = recaps[k].j;
    const int r = gridmap(i,j);
    dvObsRegionCaps(r) += recaps[k].returns;
    tObsCaps += recaps[k].returns;
  }
  clogf << "\nTag recaptures by region:" << endl;
  for (int k = 1; k <= ngrid; k++)
  {
    clogf << setw(3) << k 
          << setw(12) << setprecision(5) << dvRegionCaps[k]
          << setw(12) << setprecision(0) << dvObsRegionCaps[k] << endl;
  }
  clogf << "\nTotal recaptures: predicted " 
        << setprecision(5) << tcaps 
        << setprecision(0) << "; observed  " << tObsCaps << endl;
  //----------------------------------------------------

  //adstring prn_name = pathname.getRoot() + ".atc";
  //char prn_name[80];
  //ostrstream ss(prn_name,80);
  std::ostringstream ss;
  ss << "fit" << setw(3) << setfill('0') << hex << m_ipar[8]
	  << "_P" << pool_tags << "_pta.csv" << '\0';
  prnstream prn(ss.str().c_str());
  //if (!prn)
  //{
  //  cerr << "Error opening prn_name" << endl;
  //  exit(1);
  //}
  cout << "predicted tag attrition curve" << endl;
  prn << "predicted tag attrition curve" << endl;
  prn << "months at" << "aggregate";
  int prn_cols = ncolumns; //0;

  //if (m_ipar13)
  if (pool_tags == 0)
  {
    //prn_cols = nrelease;
    prn << "disaggregated release group" << endl;
    clogf << "by disaggregated release group" << endl;
  }
  if (pool_tags == 1)
  {
    //prn_cols = nfleet;
    prn << "recapture fleet" << endl;
    clogf << "by recapture fleet" << endl;
  }
  else if (pool_tags == 2)
  {
    //prn_cols = nrelease;
    prn << "release group aggregated by release date" << endl;
    clogf << "release group aggregated by release date" << endl;
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
  }  prn << endl;

  for (int month = 0; month <= max_month; month++)
  {
    prn << month << value(dvTotalReturns(month));
    clogf << month <<" "<< dvTotalReturns(month);
    for (int cohort = 0; cohort <= prn_cols; cohort++)
    {
      prn << value(dmTac(month, cohort));
      clogf <<"," << dmTac(month, cohort);
    }
    prn << endl;
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

}//End par_t_reg::simulate(JNIEnv*,jobject,recaptype_vector&,int)

void write_xyz(const dvar_matrix& m, const par_t_reg* p, 
               const adstring id, const int fit, const year_month date)
{
  //char buf[80];
  //ostrstream ss(buf,80);
  std::ostringstream ss;
#ifdef unix
  ss << "xyz/"
#else
  ss << "xyz\\"
#endif
       << id << "-" << setw(3) << setfill('0') << hex << fit << dec
             << "-" << setw(2) << setfill('0') << date.get_year()
                    << setw(2) << setfill('0') << date.get_month_value()
      << ".xyz" << ends;
  ofstream dout(ss.str().c_str());

  if (dout)
  {
    const int i1 = m.rowmin();
    const int i2 = m.rowmax();
    for (int i = i1; i <= i2; i++)
    {
      const double x = p->index_to_long(i);
      const int j1 = m(i).indexmin();
      const int j2 = m(i).indexmax();
      for (int j = j1; j <= j2; j++)
      {
        const double y = p->index_to_lat(j);
        dout << x << " " << y << " " << 10.0*m(i,j) << endl;
      }
    }
  }
}

void write_bitmap(jnigraphics2* jni, const adstring id, const int fit, const year_month date, const int panel)
{
  //char buf[80];
  //ostrstream ss(buf,80);
  std::ostringstream ss;
#ifdef unix
  ss << "bitmaps/"
#else
  ss << "bitmaps\\"
#endif
       << id << "-" << setw(3) << setfill('0') << hex << fit << dec
             << "-" << setw(2) << setfill('0') << date.get_year()
                    << setw(2) << setfill('0') << date.get_month_value()
      << ".png" << ends;

  jni->save_png(ss.str().c_str(),640,480,panel);
}

void write_bitmap(jnigraphics2* jni, const adstring id, const int fit, const int panel)
{
  //char buf[80];
  //ostrstream ss(buf,80);
  std::ostringstream ss;
#ifdef unix
  ss << "bitmaps/"
#else
  ss << "bitmaps\\"
#endif
       << id << "-" << setw(3) << setfill('0') << hex << fit << dec
      << ".png" << ends;

  jni->save_png(ss.str().c_str(),1280,960,panel);
}

