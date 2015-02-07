//$Id: tagest.tpl 3208 2014-08-30 06:54:21Z jsibert $

  /// \file tagest.tpl ADMB template file for tagest.
  ///  \ingroup TPL

GLOBALS_SECTION
  #include "intersav.h"
  #include "fish_rec.h"
  #include "coff_t.h"
  #include "par_t_reg.h"
  #include "par_t_nn.h"
  #include "trace.h"
  #include <time.h>
  #include "prnstrem.h"
  #include <fenv.h>

  //#include <sys/sysinfo.h>
  extern double min_nb_par;
  extern double max_nb_par;

  void pad(); // function prototype in case it is not declared in a header file

  #include <factoral.h>
  factorial_class FACT(100);
  const double maximum_like_cont = 10000.0;

  setsaveinterflag interflag;
  int saveinterflag = MM_intersave::ADMB_mode;

  int _global_report_flag = 0;

  ofstream clogf;
  adstring banner;

  // some timers
  time_t start_time, end_time;
  clock_t c_start_time, c_end_time;
  clock_t usefun1, usefun2;
  clock_t fcomp1, fcomp2; 

  #ifdef __CYGWIN32__
  void ad_comm::get_slave_assignments() {;} // does nothing; satisfies reference
  #endif

  //template class par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
  //template class par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
  //template class par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>;

  //template <typename MATRIX1, typename MATRIX2> 
  //void resid_comp(const MATRIX1& obs, const MATRIX2& pred, const MATRIX1& like, const int report);

  template <typename MATRIX, typename DOUBLE>
  void velpen(const MATRIX &u, const MATRIX &v, const double xweight, const double yweight, DOUBLE &penalty);

  template <typename MATRIX, typename DOUBLE>
  void sigpen(const MATRIX& s, const double weight, DOUBLE &penalty);

  template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
  void get_effort_array(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);

  template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
  int negative_tag_penalty(const MATRIX& tags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& pen);

  template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
  void total_tag_penalty(const MATRIX& tags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& pen);

  template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
  void velpen(par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& temp, year_month& date);

  template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
  void sigpen(par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& temp, year_month& date);

  template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
  void poisson_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, d3_array& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort);

  template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
  void negative_binomial_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, d3_array& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort, VECTOR& a, const int naa, const int nb_scale);

  template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
  void exponential_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort);


  template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
  void computeLikelihood(MATRIX& release,
                       year_month& date,
                       const int cohort,
                       recaptype_vector& recaps,
                       const int nrec,
                       MATRIX& tot_mort,
                       ivector& effort_occured,
                       D3_ARRAY& fish_mort,
                       const int nfleet,
                       DOUBLE& plike,
                       par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param);
  adstring make_banner(const adstring& program_name);

  dvar_vector beta_pdf(const dvar_vector& x, const dvariable& a, const dvariable& b);


DATA_SECTION
  !!  ad_comm::change_datafile_name("file_nam.tmp");
  init_adstring filename;

  int m;
  int n;
  int nfleet;
  int fleet0;
  int last_cohort;
  int nrecapture_dev;
  int nrec;
  int ncohort;

  ivector jlb1;
  ivector jub1;
  3darray effort;
  ivector effort_occured;
  imatrix jlbm;
  imatrix jubm;
  ivector ilbv;
  ivector iubv;
  ivector jlb;
  ivector jub;

  //3darray Recaps1;   // recaptures with    fishing effort
  //3darray Recaps0;   // recaptures without fishing effort
  //3darray ObsRecaps; // used in the likelihood computation
  3darray obs_recaps; 
  4darray effort_proxy;

  int attr_index;
  int hist_index;
  int max_attr_index;
  int max_hist_index;
  imatrix obs_attr;
  imatrix obs_hist;
  vector pred_attr;
  vector pred_hist;
  3darray residuals;

PARAMETER_SECTION
 LOCAL_CALCS
    if (par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::createInstance(filename) == 0) 
    {
    }

    par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
    m = param.get_m();
    n = param.get_n();
    TTRACE(m,n)
    nfleet = param.get_nfleet();
    fleet0 = param.fleet0;

    // set some parameters to control minimizer display
    maxfn  = param.m_ipar(1);
    iprint = param.m_ipar(2);
    crit   = param.m_ipar(3)/100.;
    imax   = param.m_ipar(4);
    scroll_flag = param.m_ipar(7);
    ifn    = 0;


  if (indexed_regional_fishery_record::createInstance(filename) == 0) { }
  indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;
  if (recaptype_vector::createInstance(filename, *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance, irfr) == 0) { }
  recaptype_vector &recaps = *recaptype_vector::instance;
  const int nrec = recaps.getTotalRecords();
  if (nrec <= 0) 
  {
    cerr << "Error: No recapture data found." << endl;
  }

  nrecapture_dev = 0;
  if (param.m_ipar[13] == 2) 
  {
    last_cohort = param.collapse_cohorts(recaps, nrec);
    //nrecapture_dev = last_cohort;

  }
  const int phase_recapture_dev = param.m_ipar(55) > 0 ? 1 : -1;
  nrecapture_dev = param.duration;
  const int phase_tag_surv = param.m_ipar(92) > 0 ? 1 : -1;

  //ivector effortless = recaps.count_effortless_recaps(last_cohort,param.fleet_names);
  //clogf << "\nRecaptures WITHOUT fishing effort: " << effortless(0) << endl;
  //clogf << "Recaptures  with   fishing effort: " << effortless(1) << endl;
  //clogf << "Total recaptures:                  " << effortless(2) <<" \n" << endl;

    //recaps.tabulate_effortless_recaps(clogf,param.fleet_names);


    param.executable = adstring(argv[0]);
    ivector phaseQ(fleet0,nfleet);
    phaseQ = -1;

    if (param.m_ipar(25) == 2)
    {
      for (int f = fleet0; f <= nfleet; f++)
      {
        TTRACE(f,phaseQ(f))
        phaseQ(f) = 1;
      }
    }
    else if (param.m_ipar(25) == 1)
    {
      phaseQ(fleet0) = 1;
      for (int f = (fleet0+1); f <= nfleet; f++)
        phaseQ(f) = -1;
    }
    else 
    {
       cerr << "Unknown catchability option in flag 25 " << param.m_ipar(25) << endl;
       ad_exit(1);
    }
    
    const int phaseMort = param.m_ipar(22) == 1 ? 1 : -1;
    const int phaseSpecial_mort_phase = param.m_ipar(22) == 1 && param.m_ipar(19) > 0 ? 1 : -1;
    const int phaseR_rates = param.m_ipar(28) == 1 ? 1 : -1;
    const int phaseNb_par = param.m_ipar(11) == 24 ? 1 : -1;//watchout m_ipar == 4 || 14
    //const int phaseUV = param.m_ipar(23) == 1 ? 1 : -1;
    //const int phaseSigma = param.m_ipar(21) == 2 ? 1 : -1;// won't work for uniform D
    const int phaseSlopeU = param.m_ipar(71) == 1 ? 1 : -1;
    const int phaseOffsetU = param.m_ipar(72) == 1 ? 1 : -1;
    const int phaseSlopeV = param.m_ipar(73) == 1 ? 1 : -1;
    const int phaseOffsetV = param.m_ipar(74) == 1 ? 1 : -1;
    const int phaseSlopeSigma = param.m_ipar(21) != 1 && param.m_ipar(75) == 1 ? 1 : -1;
    const int phaseOffsetSigma = param.m_ipar(21) != 1 && param.m_ipar(76) == 1 ? 1 : -1;
    const int phaseUniformSigma = param.m_ipar(21) == 1 ? 1 : -1;
    int phaseWeights = -1;
    int phaseBias = -1;
    param.m_ipar(5) = 0;// Number of calls to fgcomp
 END_CALCS

  init_bounded_number mort(param.minmort,param.maxmort,phaseMort);
  !! TTRACE(phase_recapture_dev,recapture_dev)

  !! TRACE(phaseQ)
  !! TTRACE(param.minq,param.maxq)
  init_bounded_number_vector q(fleet0,nfleet,param.minq,param.maxq,phaseQ);
  init_bounded_dev_vector recapture_dev(0,nrecapture_dev,-8.0,8.0,phase_recapture_dev);
  init_bounded_vector tag_surv_prop(1,last_cohort,0.0,1.0,phase_tag_surv);
  init_bounded_number mean_tag_surv_prop(0.0,1.0,phase_tag_surv);
  number tag_surv_prob_a;
  number tag_surv_prob_b;

  //number mean_tag_surv_prop;
  number var_tag_surv_prop;
  //init_bounded_number tag_surv_prob_a(1.0,20,phase_tag_surv);
  //init_bounded_number tag_surv_prob_b(1.0,20,phase_tag_surv);

  init_bounded_number special_mort(param.minmort,param.maxmort,phaseSpecial_mort_phase);
  init_bounded_vector report_rates(fleet0,nfleet,param.minr_rates,param.maxr_rates,phaseR_rates);

  sdreport_vector average_fishing_mortality(fleet0,nfleet);
  sdreport_number total_fishing_mortality;
  sdreport_number total_domain_mortality;
  likeprof_number natural_mortality;
  //
  // ----------------------------------------------------------
  //
  // ALL variables need to be allocated and given initial values
  // even if they are not "active"
  //
  // ----------------------------------------------------------
  //
  init_bounded_vector nb_par(fleet0,nfleet,min_nb_par,max_nb_par,phaseNb_par);

  !! if (param.isRegional()) 
  !! {
  !!  par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& regional = (par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)param;
  !!  const int nseason = regional.get_nseason();
  !!  const int ngrid = regional.get_ngrid();

  !!  dmatrix minvel(1,nseason,1,ngrid);
  !!  dmatrix maxvel(1,nseason,1,ngrid);
  !!  minvel = param.minvel;
  !!  maxvel = param.maxvel;
  !!  imatrix phaseUV(1,nseason,1,ngrid);
  !!  phaseUV = param.m_ipar(23) == 1 ? 1: -1;
  !!  cout << "stratum_active: " << regional.stratum_active << endl; 
  !!  for (int f = 1; f <= ngrid; f++)
  !!  {
  !!     for (int s = 1; s <= nseason; s++)
  !!     { 
  !!         if (param.m_ipar(23) == 1 && regional.stratum_active(s,f) >= 10)
  !!         {
  !!             phaseUV(s,f) = 1;
  !!         }
  !!         else
  !!         {
  !!             phaseUV(s,f) = -1;
  !!         }
  !!     }
  !!  }
  !!  cout  << "phaseUV: " << phaseUV << endl;
  !! 
  init_bounded_number_matrix seasonGridU(1,nseason,1,ngrid,minvel,maxvel,phaseUV);
  init_bounded_number_matrix seasonGridV(1,nseason,1,ngrid,minvel,maxvel,phaseUV);

  !!  dmatrix minsig(1,nseason,1,ngrid);
  !!  dmatrix maxsig(1,nseason,1,ngrid);
  !!  minsig = param.minsig;
  !!  maxsig = param.maxsig;
  !!  imatrix phaseSigma(1,nseason,1,ngrid);
  !!  phaseSigma = param.m_ipar(21) == 2 ? 1 : -1;
  !!  for (int f = 1; f <= ngrid; f++)
  !!  {
  !!     for(int s = 1; s <= nseason; s++)
  !!     {
  !!        if ( param.m_ipar(21) == 2 && (regional.stratum_active(s,f)== 1 || regional.stratum_active(s,f)== 11))
  !!        {
  !!           phaseSigma(s,f) = 1;
  !!        }
  !!        else
  !!        {
  !!           phaseSigma(s,f) = -1;
  !!        }
  !!     }
  !!  }
  !!  cout  << "phaseSigma: " << phaseSigma << endl;
  init_bounded_number_matrix seasonGridSigma(1,nseason,1,ngrid,minsig,maxsig,phaseSigma);

  !!  if (param.m_ipar(84) == 1)
  !!  {
  //!!    const int phaseSus_spd = param.m_ipar(86) == 1 ? 1 : -1;
  //      init_bounded_number sus_spd(param.minsus_spd,param.maxsus_spd,phaseSus_spd);
  //!!    sus_spd = value(regional.get_sus_spd());
  !!    const int phaseChi = param.m_ipar(86) == 1 ? 1 : -1;
        init_bounded_number chi(param.minchi,param.maxchi,phaseChi);
  //!!    chi = value(regional.get_chi());
  !!  }
  !!  if (param.m_ipar(85) == 1) 
  !!  {
  //!!     ivector phaseFAD(1,ngrid);
  //!!     phaseFAD = -1;
  //!!     for (int f = 1; f <= ngrid; f++)
  //!!     {
  //!!        phaseFAD(f) = regional.fadmap_stratum_active_d(f); // == 1 ? 1 : -1; 
  //!!     }
  !!     ivector phaseMin_D (1,ngrid); 
  !!     phaseMin_D = param.m_ipar(90) == 1 ? 1 : -1;
  !!     ivector phaseSlope (1,ngrid);
  !!     phaseSlope = param.m_ipar(88) == 1 ? 1 : -1;
  !!     ivector phaseInflection (1,ngrid);
  !!     phaseInflection = param.m_ipar(89) == 1 ? 1 : -1;
  !!     const double mininflection=0.0;
  !!     cout << "maxinflection: " << param.maxinflection << endl;  
  !!     for (int f = 1; f <= ngrid; f++)
  !!     {
  //!!        if (param.m_ipar(88) == 1 && regional.fadmap_stratum_active_d(f) >= 10)
  !!        if (param.m_ipar(88) == 1 && (regional.fadmap_stratum_active_d(f) == 10 || regional.fadmap_stratum_active_d(f) == 11 || regional.fadmap_stratum_active_d(f) >= 110))
  !!        {
  !!            if (param.maxinflection(f) == 0 )
  !!            {
  !!                cerr << "When there is no FAD (max inflection=0), phaseSlope should be -1. Turn off the active flag for slope in region " << f << endl;
  !!                ad_exit(1);
  !!            }
  !!            else
  !!            {
  !!                phaseSlope(f) = 1;
  !!            }
  !!        }
  !!        else
  !!        {
  !!            phaseSlope(f) = -1;
  !!        }
  !!     }
  !!     for (int f = 1; f <= ngrid; f++)
  !!     {
  //!!        if (param.m_ipar(89) == 1 && (regional.fadmap_stratum_active_d(f) == 1 || regional.fadmap_stratum_active_d(f) == 11))
  !!        if (param.m_ipar(89) == 1 && (regional.fadmap_stratum_active_d(f) == 1 || regional.fadmap_stratum_active_d(f) == 11 || regional.fadmap_stratum_active_d(f) == 101 || regional.fadmap_stratum_active_d(f) == 111))
  !!        {
  !!            if (param.maxinflection(f) == 0 )
  !!            {
  !!                cerr << "When there is no FAD (max inflection=0), phaseInflection, phaseSlope, and phaseSigma should be -1. Turn off the active flag for them in region " << f << endl;
  !!                ad_exit(1);
  !!            }
  !!            else
  !!            {
  !!                phaseInflection(f) = 1;
  !!            }
  !!        }   
  !!        else
  !!        {
  !!            phaseInflection(f) = -1;
  !!        }
  !!     }
  !!     for (int f = 1; f <= ngrid; f++)
  !!     {
  !!        if (param.m_ipar(90) == 1 && regional.fadmap_stratum_active_d(f) >= 100)
  !!        {
  !!            if (param.maxinflection(f) == 0 )
  !!            {
  !!                cerr << "When there is no FAD (max inflection=0), phaseInflection, phaseSlope, and phaseSigma should be -1. Turn off the active flag for them in region " << f << endl;
  !!                ad_exit(1);
  !!            }
  !!            else
  !!            {
  !!                phaseMin_D(f) = 1;
  !!            }
  !!        }   
  !!        else
  !!        {
  !!            phaseMin_D(f) = -1;
  !!        }
  !!     }
         //init_number_vector min_D(1,ngrid,phaseMin_D);
         init_bounded_number_vector min_D(1,ngrid,param.minmin_D,param.maxmin_D,phaseMin_D);
         init_bounded_number_vector logslope(1,ngrid,param.minlogslope,param.maxlogslope,phaseSlope);
         init_bounded_number_vector inflection(1,ngrid,mininflection,param.maxinflection,phaseInflection);
  !!     cout << "regional.fadmap_stratum_active_d: " << regional.fadmap_stratum_active_d << endl;
  !!     cout << "phaseMin_D: " << phaseMin_D << endl;
  !!     cout << "phaseSlope: " << phaseSlope << endl;
  !!     cout << "phaseInflection: " << phaseInflection << endl;
  !!     cout << "param.minlogslope: " << param.minlogslope << endl;
  !!     cout << "param.maxlogslope: " << param.maxlogslope << endl;
  !!     cout << "mininflection: " << mininflection << endl;
  !!     cout << "param.maxinflection: " << param.maxinflection << endl;
  //!!     for (int f = 1; f <= ngrid; f++)
  //!!     {
  //!!       min_D(f) = value(regional.get_min_D()(f));
  //!!       logslope(f) = value(regional.get_logslope()(f));
  //!!       inflection(f) = value(regional.get_inflection()(f));
  //!!       cout << "min_D: " << min_D << endl;  
  //!!       cout << "logslope: " << logslope << endl;  
  //!!       cout << "inflection: " << inflection << endl;  
  //!!     }
  !!  }//if(m_ipar(85)==1)
  !!  else if (param.m_ipar(85) == 2) 
  !!  {
  !!    const int phaseGamma = param.m_ipar(87) == 1 ? 1 : -1;
        init_bounded_number loggamma(param.minloggamma,param.maxloggamma,phaseGamma);
  //!!    loggamma = value(regional.get_loggamma());
  //!!    cout << "loggamma: " << loggamma << endl;
  !!  }//else if(m_ipar(85)==2)
  sdreport_matrix obrienSigma(1,nseason,1,ngrid);
  sdreport_matrix ddr(1,nseason,1,ngrid); ///< daily diffusion radius
  
  sdreport_vector slope(1,ngrid);
  sdreport_number L;
  sdreport_number U;
  sdreport_number gamma;

  

 LOCAL_CALCS
    for (int season = 1; season <= nseason; season++) 
    {
      for (int grid = 1; grid <= ngrid; grid++) 
      {
        seasonGridU(season, grid) = value(regional.get_usergrid_u(season, grid));
        seasonGridV(season, grid) = value(regional.get_usergrid_v(season, grid));
        seasonGridSigma(season, grid) = value(regional.get_usergrid_sigma(season, grid));
      }
    }
    if (param.m_ipar(84) == 1)
    {
      chi = value(regional.get_chi());
      cout << "chi: " << chi << endl;
    }
    if (param.m_ipar(85) == 1)
    {
       for (int f = 1; f <= ngrid; f++)
       {
         min_D(f) = value(regional.get_min_D()(f));
         logslope(f) = value(regional.get_logslope()(f));
         inflection(f) = value(regional.get_inflection()(f));
         cout << "min_D: " << min_D << endl;  
         cout << "logslope: " << logslope << endl;  
         cout << "inflection: " << inflection << endl;  
       }
     }
    if (param.m_ipar(85) == 2)
    {
      loggamma = value(regional.get_loggamma());
      cout << "loggamma: " << loggamma << endl;
    }
    
 END_CALCS

  !! }
  !! else if (param.isNeuralNet()) 
  !! {
  // -------------------------------
  !!  par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>& neuralnet = (par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)param;

  init_bounded_number u_slope(neuralnet.min_slope,neuralnet.max_slope,phaseSlopeU);
  init_bounded_number u_offset(neuralnet.min_offset,neuralnet.max_offset,phaseOffsetU);
  init_bounded_number v_slope(neuralnet.min_slope,neuralnet.max_slope,phaseSlopeV);
  init_bounded_number v_offset(neuralnet.min_offset,neuralnet.max_offset,phaseOffsetV);
  init_bounded_number sigma_slope(neuralnet.min_slope,neuralnet.max_slope,phaseSlopeSigma);
  init_bounded_number sigma_offset(neuralnet.min_offset,neuralnet.max_offset,phaseOffsetSigma);
  init_bounded_number uniform_sigma(neuralnet.minsig,neuralnet.maxsig,phaseUniformSigma);

 LOCAL_CALCS
    int numWeights = 0;
    for (int n = 1; n <= neuralnet.num_nn; n++) 
    {
      if (neuralnet.nn(n).nn_ipar(5) == 1) 
      {
        phaseWeights = 1;
        ivector nrow = neuralnet.nn(n).get_nrow();
        ivector ncol = neuralnet.nn(n).get_ncol();
        const int num_levels =  neuralnet.num_levels(n) - 1;
        for (int level = 1; level < num_levels; level++) 
        {
          numWeights += nrow(level) * ncol(level);
        }
      }
    }

    int numDiff_hwts = 0;
    for (int n = 1; n <= neuralnet.num_nn; n++) 
    {
      if (neuralnet.nn(n).nn_ipar(5) == 1) 
      {
        const int level = neuralnet.num_levels(n) - 1;
        ivector nrow = neuralnet.nn(n).get_nrow();
        ivector ncol = neuralnet.nn(n).get_ncol();
        numDiff_hwts += nrow(level) * ncol(level);
      }
    }

    int numBias = 0;
    for (int s = 1; s <= neuralnet.num_nn; s++) //biases
    {
      if (neuralnet.nn(s).nn_ipar(6)) 
      {
        phaseBias = 1;
        ivector nrow = neuralnet.nn(s).get_nrow();
        for (int level = 1; level < neuralnet.num_levels(s); level++) 
        {
          numBias += nrow(level);
        }
      }
    }

 END_CALCS

  init_bounded_vector weights(1,numWeights,neuralnet.min_weight,neuralnet.max_weight,phaseWeights);
  init_bounded_vector diff_hwts(1,numDiff_hwts,neuralnet.min_diff_hwts,neuralnet.max_diff_hwts,phaseWeights);
  init_bounded_vector bias(1,numBias,neuralnet.min_bias,neuralnet.max_bias,phaseBias);

 LOCAL_CALCS
    u_slope = value(neuralnet.u_slope);
    u_offset = value(neuralnet.u_offset);
    v_slope = value(neuralnet.v_slope);
    v_offset = value(neuralnet.v_offset);
    sigma_slope = value(neuralnet.sig_slope);
    sigma_offset = value(neuralnet.sig_offset);
    uniform_sigma = value(neuralnet.uniform_sigma);

    int index = 1;
    for (int s = 1; s <= neuralnet.num_nn; s++) 
    {
      if (neuralnet.nn(s).nn_ipar(5) == 1) 
      {
        ivector nrow = neuralnet.nn(s).get_nrow();
        ivector ncol = neuralnet.nn(s).get_ncol();
        dvar3_array w = neuralnet.nn(s).get_weights();
        for (int level = 1; level < neuralnet.num_levels(s) - 1; level++) 
        {
          for (int i = 1; i <= nrow(level); i++) 
          {
            for (int j = 1;  j <= ncol(level); j++) 
            {
              weights(index) = value(w(level,i,j));
              index++;
            }
          }
        }
      }
    }

    index = 1;
    for (int s = 1; s <= neuralnet.num_nn; s++) //get the weights
    {
      if (neuralnet.nn(s).nn_ipar(5) == 1) 
      {
        ivector nrow = neuralnet.nn(s).get_nrow();
        ivector ncol = neuralnet.nn(s).get_ncol();
        dvar_matrix d = neuralnet.nn(s).get_diff_hwts();
        for (int i = 1; i <= nrow(neuralnet.num_levels(s) - 1); i++) 
        {
          for (int j = 1; j <= ncol(neuralnet.num_levels(s) - 1); j++) 
          {
            diff_hwts(index) = value(d(i, j));
            index++;
          }
        }
      }
    }

    index = 1;
    for (int s = 1; s <= neuralnet.num_nn; s++) //biases
    {
      if (neuralnet.nn(s).nn_ipar(6)) 
      {
        ivector nrow = neuralnet.nn(s).get_nrow();
        dvar_matrix b = neuralnet.nn(s).get_bias();
        for (int level = 1; level < neuralnet.num_levels(s); level++) 
        {
          for (int i = 1; i <= nrow(level); i++) 
          {
            bias(index) = value(b(level, i));
            index++;
          }
        }
      }
    }
 END_CALCS

  !!
  !! }
  !!

 LOCAL_CALCS
    if (param.m_ipar(25) >= 2)
    {
      for (int f = fleet0; f <= nfleet; f++)
          q(f) = param.q(f);
    }
    else if  (param.m_ipar(25) == 1)
    {
      q(fleet0) = param.q(fleet0);
      for (int f = fleet0+1; f <= nfleet; f++)
          q(f) = param.q(1);
    }
    mort = value(param.mort);
    special_mort = value(param.special_mort);
    report_rates = value(param.report_rates);
    //if (phaseNb_par == 1) 
    if ((param.m_ipar[11]==24) || (param.m_ipar[11]==25) )
    {
      nb_par = value(param.nb_par);
    }

    TRACE(recapture_dev)   
    for (int k = 1; k <= nrecapture_dev; k++)
       recapture_dev(k) = param.recapture_dev(k);
    TRACE(recapture_dev)   
 END_CALCS

 LOCAL_CALCS
        jlb1.allocate(0,m+1);
        jub1.allocate(0,m+1);
        for (int i = 1; i <= m; i++)
        {
           jlb1(i) = param.jlb(i) - 1;
           jub1(i) = param.jub(i) + 1;
        }
        jlb1(0) = 0;
        jlb1(m+1) = 0;
        jub1(0) = n+1;
        jub1(m+1) = n+1;
 END_CALCS

        matrix u(0,m+1,jlb1,jub1);
        matrix v(0,m+1,jlb1,jub1);
        matrix sigma(0,m+1,jlb1,jub1);

 LOCAL_CALCS
    jlbm.allocate(0,nfleet,1,m);
    jubm.allocate(0,nfleet,1,m);
    ilbv.allocate(0,nfleet);
    iubv.allocate(0,nfleet);
    for (int s = 0; s <= nfleet; s++)
    {
      ilbv(s) = 1;
      iubv(s) = m;
      for (int i = 1; i <= m; i++)
      {
        jlbm(s,i) = param.jlb(i);
        jubm(s,i) = param.jub(i);
      }
    }


    effort.allocate(0, nfleet, ilbv, iubv, jlbm, jubm);
    effort_occured.allocate(0, nfleet);
    jlb = param.jlb;
    jub = param.jub;

    // 0th element of slice 0 is total for that slice
    //Recaps0.allocate(0, nfleet, ilbv, iubv, jlbm, jubm);
    //Recaps0.initialize();
    //Recaps1.allocate(0, nfleet, ilbv, iubv, jlbm, jubm);
    //Recaps1.initialize();
    //ObsRecaps.allocate(0, nfleet, ilbv, iubv, jlbm, jubm);
    //ObsRecaps.initialize();
    obs_recaps.allocate(0, nfleet, ilbv, iubv, jlbm, jubm);
    obs_recaps.initialize();

    residuals.allocate(0,m+1,jlb1,jub1,1,2);

 END_CALCS
  3darray PredRecaps(0,nfleet,ilbv,iubv,jlbm,jubm);
  3darray fishing_mortality(0,nfleet,ilbv,iubv,jlbm,jubm);
  //3darray FishMort0(0,nfleet,ilbv,iubv,jlbm,jubm);
  matrix total_mortality(1,m,jlb,jub);
  matrix release(1,m,jlb,jub);

  objective_function_value likelihood;

PRELIMINARY_CALCS_SECTION
  /*
  The functions set_stepnumber() and set_stepsize() can be used to modify the number
  of points used to approximate the profile likelihood, or to change the stepsize between the
  points. This can be carried out in the PRELIMINARY_CALCS_SECTION. If u has been declared
  to be of type likeprof_number,
  PRELIMINARY_CALCS_SECTION
  u.set_stepnumber(10); // default value is 8
  u.set_stepsize(0.2); // default value is 0.5
  will set the number of steps equal to 21 (from -10 to 10) and will set the step size equal to
  0.2 times the estimated standard deviation for the parameter u.
  */
 
  clogf << "In PRELIMINARY_CALCS" << endl;

  natural_mortality.set_stepnumber(4); // default value is 8
  natural_mortality.set_stepsize(0.1); // default value is 0.5

  //if (par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::createInstance(filename) == 0) 
  //{
  //   cerr << "Unable to create par_t instance in  PRELIMINARY_CALCS_SECTION" << endl;
  //   exit(1);
  //}

  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  recaptype_vector &recaps = *recaptype_vector::instance;
  indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;

  year_month final_date = param.get_tr_date(param.nrelease) + param.nmonth;
  TTRACE(param.start_date,final_date)
  max_hist_index = param.duration;
  max_attr_index = param.nmonth + 2;
  TTRACE(max_attr_index,max_hist_index)
  obs_attr.allocate(0,max_attr_index,1,3);
  obs_hist.allocate(0,max_hist_index,1,3);
  pred_attr.allocate(0,max_attr_index);
  pred_hist.allocate(0,max_hist_index);
  
    TRACE(max_hist_index)
    //effort_proxy.allocate(0,max_hist_index,0, nfleet, ilbv, iubv, jlbm, jubm);
    effort_proxy.allocate(0,max_hist_index);
    TTRACE(effort_proxy.hslicemin(),effort_proxy.hslicemax())
    for (int h = 0; h <= max_hist_index; h++)
    {
      effort_proxy(h).allocate(0, nfleet, ilbv, iubv, jlbm, jubm);
    }
    TTRACE(effort_proxy.hslicemin(),effort_proxy.hslicemax())
    int hs1 = effort_proxy.hslicemin();
    TTRACE(effort_proxy(hs1).slicemin(),effort_proxy(hs1).slicemax())

    //recaps.make_effort_proxy(effort_proxy, param.global_start_date, param.fleet_names);
    //void indexed_regional_fishery_record::
    //  get_effort(adstring_array pfleets, d4_array& effort,
    //  year_month& pstart_date)
    irfr.get_effort(param.fleet_names, effort_proxy,param.global_start_date);


  for (int i = fleet0; i <= nfleet; i++)
     average_fishing_mortality = 0.0;

  total_fishing_mortality = 0.0;
  total_domain_mortality = 0.0;
  natural_mortality = 0.0;
  param.recapture_dev_weight = param.m_ipar(55)/100.0;
  param.tag_surv_prop_weight = param.m_ipar(92);
  if (param.m_ipar(91) > 0)
     tag_surv_prop = param.tag_surv_prop;
  //tag_surv_prob_a = 1.1;
  //tag_surv_prob_b = 1.1;
  //TTRACE(tag_surv_prob_a,tag_surv_prob_b)
  /*
  mean_tag_surv_prop = tag_surv_prob_a/(tag_surv_prob_a+tag_surv_prob_b);
  sd_tag_surv_prop = sqrt(tag_surv_prob_a*tag_surv_prob_b/
       square(tag_surv_prob_a+tag_surv_prob_b)*(tag_surv_prob_a+tag_surv_prob_b+1));
  TTRACE(mean_tag_surv_prop,sd_tag_surv_prop);
  */

  for (int i = slope.indexmin(); i <= slope.indexmax(); i++)
  {
     slope(i) = 0.0;
  }

  L = 0.0;
  U = 0.0;
  HERE


  /*
  if (recaptype_vector::createInstance(filename, *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance, irfr) == 0) { }
  recaptype_vector &recaps = *recaptype_vector::instance;
  const int nrec = recaps.getTotalRecords();
  if (nrec <= 0) 
  {
    cerr << "Error: No recapture data found." << endl;
  }

  if (param.m_ipar[13] == 2) 
  {
    last_cohort = param.collapse_cohorts(recaps, nrec);
  }

  ivector effortless = recaps.count_effortless_recaps();
  clogf << "Recaptures WITHOUT fishing effort: " << effortless(0) << endl;
  clogf << "Recaptures  with   fishing effort: " << effortless(1) << endl;
  clogf << "Total recaptures:                  " << effortless(2) << endl;
  */

  //recaptype_vector &recaps = *recaptype_vector::instance;

   #ifdef OLD_DO_KLUDGE
   if (param.m_ipar[43]) 
   {
     adstring phdo("phdo");
     int s_phdo = irfr.set_effort_to_recaps(phdo, recaps, nrec, param);
     cout << s_phdo << " effort records substituted for " << phdo << endl;
     clogf << s_phdo << " effort records substituted for " << phdo << endl;
   }
   if (param.m_ipar[44]) 
   {
     adstring iddo("iddo");
     int s_iddo = irfr.set_effort_to_recaps(iddo, recaps, nrec, param);
     cout << s_iddo << " effort records substituted for " << iddo << endl;
     clogf << s_iddo << " effort records substituted for " << iddo << endl;
   }
   #else // new DO effort kludge
   for (int f = fleet0; f <= nfleet; f++)
   {
      if (param.DomesticFleetKludgeFlag(f))
      {
         int nsub =  irfr.set_effort_to_recaps(param.fleet_names(f), recaps, nrec, param);
         cout << nsub << " effort records substituted for " << param.fleet_names(f) << endl;
         clogf << nsub << " effort records substituted for " << param.fleet_names(f) << endl;
      }
   }
   #endif
   param.set_fishing_effort_normalization(irfr);
   if (irfr.is_normalized() == false)
   {
     clogf << "Fishing effort not normalized." << endl;
   }
   else
   {
     clogf << "Fishing effort is normalized." <<endl;
   }
   average_fishing_mortality.initialize(); 
   param.m_ipar[5] = 0;
   param.savefit(0.0, 0, 0.0, ".ppp");
   if (coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::createInstance(*par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance) == 0) 
   {
   }
   param.set(*this);

   clogf << "\ninitial_params:"<< endl;
   clogf << "index name phase active value" << endl;
   for (int i=0;i< initial_params::num_initial_params;i++)
   {
      initial_params& t = *initial_params::varsptr[i];
      clogf << (i+1) << "  " << t.label() << "  ";
      clogf << "  " << t.get_phase_start();
      clogf << "  " << active(t) << "  ";
      t.save_value(clogf,15);
   }
   clogf << endl;

   HERE
   clogf << "Finished PRELIMINARY_CALCS" << endl;

PROCEDURE_SECTION
  usefun1 = clock();
  fcomp1 = fcomp2;
  fcomp2 = clock();
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  recaptype_vector &recaps = *recaptype_vector::instance;
  
  reset();
  release.initialize();
  obs_hist.initialize();
  pred_hist.initialize();
  residuals.initialize();

  likelihood -= param.fpen;

  param.add_total_bounds_penalty(param.fpen);

  year_month save_start_date = param.start_date;

  ncohort = param.nrelease;
  int cohort = 0;
  int last_rn = 0;
  int c1 = 1;
  int c2 = ncohort;
  int pool_tags = param.m_ipar[13];
  if (pool_tags == 1)
  {
    cohort = 0;
    c2 = 1;
  }
  else if (pool_tags == 2)
  {
    last_rn = 0;
    c1 = 1;
    c2 = ncohort;
    obs_attr.initialize();
    pred_attr.initialize();
  }

  int omit_months = param.m_ipar(14);
  if (omit_months > 0 && pool_tags < 1)
  {
     cerr << "Error: setting ipar(14) > 0 can only be applied to pooled cohorts" << endl;
     cerr << "  ipar(13) is currently set to " << pool_tags << endl;
     ad_exit(1);
  }
  int months_at_liberty = 0;

  indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;
  coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &coff = *coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  const int nrec = recaps.getTotalRecords();

  funnel_dvariable funnel;
  year_month global_start_date = param.global_start_date;
  year_month local_start_date = param.start_date;
  year_month final_date = param.final_date;
  for (int cc = c1; cc <= c2; cc++)
  {
    cohort ++;
    if (pool_tags == 2)
    {
      release.initialize();
      cohort = param.get_tr_cohort(cc);
      //clogf << "\nReleasing tags for cohort " << cohort << endl;
      local_start_date = param.get_tr_date(cc);
      final_date = local_start_date + param.nmonth;
      param.start_date = local_start_date;
      release.initialize();
      while ((last_rn < ncohort) && (cohort == param.get_tr_cohort(last_rn+1)))
      {
        last_rn++;
        int i = param.get_tr_i(last_rn);
        int j = param.get_tr_j(last_rn);
        if (param.m_ipar(92) > 0)
        { 
           release(i,j) += tag_surv_prop(cohort)*param.get_tr_tags(last_rn);
           //clogf << local_start_date << ", released "
           //      << tag_surv_prop(cohort) << " of "
           //      << param.get_tr_tags(last_rn) << "tags at " << i << "," << j << endl;
        }
        else
        {
           release(i,j) += param.get_tr_tags(last_rn);
           //clogf << local_start_date <<", released "
           //      << param.get_tr_tags(last_rn) << " at " << i << "," << j << endl;
        }
      }  // end while
      cc = last_rn;
      months_at_liberty = 0;
    }  // end if

    dvariable plike = 0;
    effort.initialize();
    effort_occured.initialize();
    //fish_mort.initialize();
    //tot_mort.initialize();
    u.initialize();
    v.initialize();
    sigma.initialize();
    coff.clean();
    if (param.isRegional())
    {
      ad_begin_funnel();
    }

    recaps.reset_position();
    int current_season = 0;
    for (year_month date = local_start_date; date <= final_date; date++) 
    {
      attr_index = date - local_start_date;
      hist_index = date - global_start_date;
      if (pool_tags == 1)
      {
        while (last_rn < ncohort && date == param.get_tr_date(last_rn+1))
        {
          last_rn++;
          const int i = param.get_tr_i(last_rn);
          const int j = param.get_tr_j(last_rn);
          release(i,j) += param.get_tr_tags(last_rn);
          //clogf << date <<", released " << param.get_tr_tags(last_rn) << " at "
          //      << i << "," << j << endl;
        }
        months_at_liberty = 0;
      }
      months_at_liberty ++;

      if (param.isRegional())
      {
        par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& regional = (par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)param;

        if (current_season != regional.get_season(date) || param.m_ipar[83] == 1)
        {
          current_season = regional.get_season(date);
          regional.uvs_comp(u, v, sigma, date);

          dvariable penlike = 0.0;
          velpen<dvar3_array,dvar_matrix,dvar_vector,dvariable>(regional, penlike, date);
          param.add_total_uv_penalty(penlike);
          plike += penlike;

          penlike = 0.0;
          sigpen(regional, penlike, date);
          param.add_total_sigma_penalty(penlike);
          plike += penlike;

          coff.comp(param, u, v, sigma);
        }
      }
      else if (param.isNeuralNet())
      {
        par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>& neuralnet = (par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)param;
        neuralnet.uvs_comp(u, v, sigma, date);
        if (param.m_ipar[33] == 1)
        {
          const double uv_xweight = (const double)param.m_ipar[31]*1e-9;
          const double uv_yweight = (const double)param.m_ipar[32]*1e-9;
          dvariable uv_penalty = 0.0;
          velpen<dvar_matrix,dvariable>(u, v, uv_xweight, uv_yweight, uv_penalty);
          plike -= uv_penalty;
          param.add_total_uv_penalty(uv_penalty);
        }
        if (param.m_ipar[41] == 1) 
        {
          const double sig_weight = (const double)param.m_ipar[42]*1e-9;
          dvariable sigma_penalty = 0.0;
          sigpen<dvar_matrix,dvariable>(sigma, sig_weight, sigma_penalty);
          plike -= sigma_penalty;
          param.add_total_sigma_penalty(sigma_penalty);
        }
        coff.comp(param, u, v, sigma);
      }

      recaps.getreturns(obs_recaps, date, cohort);

      param.fish_mort_comp(fishing_mortality, effort_proxy(hist_index));//F=q*effort
      
      param.total_mort_comp(fishing_mortality, total_mortality);
      
      coff.b_adjust(param, total_mortality);

      for (int f = fleet0; f <= nfleet; f++)
      {
         //param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);
         param.pred_recapture_comp(PredRecaps(f), release,
                    total_mortality, fishing_mortality, f, date);
      }          

      if (months_at_liberty != omit_months)
      {
         compute_likelihood(plike);
      }

      // compute sums for attrition and history plot diagnostics
      const double tobs  = sum(obs_recaps);
      const double tobs0 = sum(obs_recaps(0));
      const double tobs1 = tobs - tobs0;

      const double tpred = sum(value(PredRecaps));
      /*
      for (int f = fleet0; f <= nfleet; f++)
      {
         //tobs += sum(ObsRecaps(0));
         //tobs0 += sum(Recaps0(0));
         tpred += sum(value(PredRecaps(f)));
      }
      */
      obs_hist(hist_index,1) += (int)tobs1;
      obs_attr(attr_index,1) += (int)tobs1;
      obs_hist(hist_index,2) += (int)tobs0;
      obs_attr(attr_index,2) += (int)tobs0;
      obs_hist(hist_index,3) += (int)tobs;
      obs_attr(attr_index,3) += (int)tobs;

      pred_hist(hist_index) += tpred;
      pred_attr(attr_index) += tpred;

      dvariable neg_tag_pen = 0;
      negative_tag_penalty<dvar3_array,dvar_matrix,dvar_vector,dvariable>(release, param, neg_tag_pen);
      param.add_total_negative_tag_penalty(neg_tag_pen);
      plike -= neg_tag_pen;

      dvariable tot_tag_pen = 0;
      total_tag_penalty<dvar3_array,dvar_matrix,dvar_vector,dvariable>(release, param, tot_tag_pen);
      param.add_total_total_tag_penalty(tot_tag_pen);
      plike -= tot_tag_pen;

      coff.adi(release,NULL);
    }
    if (param.isRegional())
    {
      funnel = plike;
      likelihood += funnel;
    }
    else if (param.isNeuralNet())
    {
      likelihood += plike;
    }
  }
  param.start_date = save_start_date;

  if (param.m_ipar(55) > 0)
  {
     param.recapture_dev_pen = -param.recapture_dev_weight*norm2(recapture_dev);
     TTRACE(likelihood,param.recapture_dev_pen)
     likelihood += param.recapture_dev_pen;
  }


  if (param.m_ipar(92) > 0)
  {
  #define VERSION_2
  #ifdef VERSION_1
     TRACE(tag_surv_prop) // runtime diagnostic
     dvariable xbar = sum(tag_surv_prop)/(double)last_cohort;
     dvariable vbar = norm2(tag_surv_prop - xbar)/(double)(last_cohort-1)+1e-8;
     if (vbar < xbar*(1.0-xbar))
     {
        //temp = ((sample.mean*(1-sample.mean))/sample.var) - 1
        dvariable tmp = ((xbar*(1.0-xbar))/vbar) - 1.0;
        //est.a = sample.mean*temp
        tag_surv_prob_a = xbar*tmp;
        //est.b = (1-sample.mean)*temp
        tag_surv_prob_b = (1.0-xbar)*tmp;
     }
     else
     {
        cerr << "xbar, vbar out of range: " << xbar << " " << vbar << endl;
        clogf << "xbar, vbar out of range: " << xbar << " " << vbar << endl;
     } 
     TTRACE(tag_surv_prob_a,tag_surv_prob_b) // runtime diagnostic
     //beta.mean <- a/(a+b)
     mean_tag_surv_prop = tag_surv_prob_a/(tag_surv_prob_a +tag_surv_prob_b);
     //beta.var <- (a*b)/(((a+b)^2)*(a+b+1))
     var_tag_surv_prop = (tag_surv_prob_a*tag_surv_prob_b)/
       (square(tag_surv_prob_a+tag_surv_prob_b)*(tag_surv_prob_a+tag_surv_prob_b+1.0));
     //TTRACE(xbar,vbar)
     //TTRACE(mean_tag_surv_prop,var_tag_surv_prop);
     param.tag_surv_prop_pen = -sum(beta_pdf(tag_surv_prop,tag_surv_prob_a,tag_surv_prob_b));

     TTRACE(likelihood,param.tag_surv_prop_pen)
     param.tag_surv_prop_pen *= 0.1*param.tag_surv_prop_weight;
     likelihood += param.tag_surv_prop_pen;
     TTRACE(likelihood,param.tag_surv_prop_pen)

  #elif defined VERSION_2
     TRACE(tag_surv_prop) // runtime diagnostic
     dvariable vbar = var(tag_surv_prop);
     dvariable xbar = mean(tag_surv_prop);
     TTRACE(xbar,vbar)
     dvariable tmp = ((xbar*(1.0-xbar))/vbar) - 1.0;
     tag_surv_prob_a = xbar*tmp;
     tag_surv_prob_b = (1.0-xbar)*tmp;
     TTRACE(tag_surv_prob_a,tag_surv_prob_b) // runtime diagnostic

     dvariable pen1 = norm2(tag_surv_prop - mean_tag_surv_prop);
     TTRACE(pen1,mean_tag_surv_prop)
    
     dvariable pen2 = 10.0*square(mean_tag_surv_prop - param.tag_surv_prop_prior);
     TRACE(pen2)

     param.tag_surv_prop_pen = -(pen1+pen2);
     param.tag_surv_prop_pen *= param.tag_surv_prop_weight;
     likelihood += param.tag_surv_prop_pen;
     TTRACE(likelihood,param.tag_surv_prop_pen)

  #else
     TRACE(tag_surv_prop) // runtime diagnostic
     TTRACE(tag_surv_prob_a,tag_surv_prob_b) // runtime diagnostic
     mean_tag_surv_prop = tag_surv_prob_a/(tag_surv_prob_a +tag_surv_prob_b);
     var_tag_surv_prop = (tag_surv_prob_a*tag_surv_prob_b)/
       (square(tag_surv_prob_a+tag_surv_prob_b)*(tag_surv_prob_a+tag_surv_prob_b+1.0));
     dvariable pen1 = norm2(tag_surv_prop - mean_tag_surv_prop);
     TTRACE(pen1,var_tag_surv_prop)

     dvariable pen2 = square(pen1-var_tag_surv_prop);
     TRACE(pen2)
    
     dvariable pen3 = 5.0*square(mean_tag_surv_prop - param.tag_surv_prop_prior); //0.76);
     TTRACE(pen3,mean_tag_surv_prop)

     param.tag_surv_prop_pen = -(pen2+pen3);
     param.tag_surv_prop_pen *= param.tag_surv_prop_weight;
     likelihood += param.tag_surv_prop_pen;
     TTRACE(likelihood,param.tag_surv_prop_pen)
  #endif // VERSION_X
  }

  likelihood *= -1;

  natural_mortality = mort;

  //if ( (ifn == 3) && (_global_report_flag == 0))
  if (iexit > 0)
  {
    //resid_comp(release, release, release, 1);
    _global_report_flag ++;
  }
  //TTRACE(iexit,_global_report_flag)

  usefun2 = clock();
  double uf_elapsed_time = (double)(usefun2-usefun1)/(double)CLOCKS_PER_SEC;
  double of_elapsed_time = (double)(fcomp2-fcomp1)/(double)CLOCKS_PER_SEC;

  clogf << "Objective function value = " << setprecision(8) << likelihood
        << "; elapsed time = " << setprecision(3) << of_elapsed_time 
        << " seconds. User function time = " << setprecision(3) 
        << uf_elapsed_time << " seconds." << endl;

 
  /*
  if(1)
  {
    TTRACE(param.start_date,final_date)
    compute_average_fishing_mortality(param.start_date,final_date);
    //exit(1);
  }
  */
  
  if (sd_phase())
  {
    TRACE(months_at_liberty)
    compute_obrien_sigma();
    compute_average_fishing_mortality(param.start_date,final_date);
    _global_report_flag = 0;
    if (param.m_ipar(83) == 1 && param.m_ipar(85) == 1)
    {
       TTRACE(param.m_ipar(83),param.m_ipar(85))
       slope=exp(logslope);
    }
    if (param.m_ipar(83) == 1 && param.m_ipar(85) == 2)
    {
       gamma=exp(loggamma);
    }
  }

  if ( ((++param.m_ipar(5) % 10) == 0) && !sd_phase())
  {
    const int nvar =  initial_params::nvarcalc();
    param.savefit(value(likelihood), nvar, likelihood.gmax, ".tmp");
  }

  /** Copies template paramters into param structure.
  This function needs to be modified whenever a new model parameter (i.e. init_ somtehing)
  is added to the PARAMETER_SECTION. It is not necessary to modify the reset functions in
  the src directory. This approach is feasible because class model_parameters in ADMB is a
  friend of class par_t.
  */
FUNCTION void reset()
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  likelihood = 0.0;
  //param.fpen = 0.0;
  param.total_uv_penalty = 0;
  param.total_sigma_penalty = 0;
  param.total_negative_tag_penalty = 0;
  param.total_total_tag_penalty = 0 ;
  param.total_bounds_penalty = 0;

  //clogf.precision(6);

  //param.q = q;
  if (param.m_ipar(25) == 2)
  {
    for (int f = fleet0; f <= nfleet; f++)
    {
      param.q(f) = q(f);
    }
  }
  else if  (param.m_ipar(25) == 1)
  {
    param.q(fleet0) = q(fleet0);
    for (int f = fleet0+1; f <= nfleet; f++)
      param.q(f) = q(fleet0);
  }

  param.mort = mort;

  if (param.m_ipar(55) > 0)
  {
     for (int c = recapture_dev.indexmin(); c <= recapture_dev.indexmax(); c++)
        param.recapture_dev(c) = recapture_dev(c);
  }

  if (param.m_ipar(92) > 0)
  {
     if (!param.tag_surv_prop)
        param.tag_surv_prop.allocate(1,last_cohort);
     
     param.tag_surv_prop = tag_surv_prop;
     param.mean_tag_surv_prop = mean_tag_surv_prop;
  }

  param.special_mort = special_mort;
  param.report_rates = report_rates;
  param.nb_par = nb_par;
  if(param.m_ipar(83)==1 && param.m_ipar(86)==1)
  {
     //param.sus_spd = sus_spd;
     param.chi = chi;
  }
  if(param.m_ipar(83)==1 && param.m_ipar(85)==2 && param.m_ipar(87)==1)
  {
     param.loggamma = loggamma;
     TRACE(loggamma)
  }

  if (param.isRegional()) 
  {
    par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& regional = (par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)param;
    // this loop is necessary because
    //   usergridU.allocate(1, nseason, 0, ngrid);
    //   usergridV.allocate(1, nseason, 0, ngrid);
    //   usergridSigma.allocate(1, nseason, 0, ngrid);
    // causing
    //   regional.usergridSigma = seasonGridSigma;
    //   regional.usergridU = seasonGridU;
    //   regional.usergridV = seasonGridV;
    // to walk out of the arrays at k = 0

    for (int k=1; k <= regional.ngrid; k++)
    {
       for (int s=1; s <= regional.nseason; s++)
       {
          if (param.m_ipar(83) == 1 && param.m_ipar(85) == 1)
          {  
             //if (regional.fadmap_stratum_active_d(k) == 1)
             //{
               regional.min_D(k) = min_D(k);
               regional.logslope(k) = logslope(k);
               regional.inflection(k) = inflection(k);
               TRACE(logslope)
               TRACE(inflection)
               TRACE(seasonGridSigma)
               TRACE(seasonGridU)
               TRACE(seasonGridV)
             //}
          }
          // this is a bit of a kludge 
          // need to implemt the seasonGrid parameters as init_number_matrices
          //if (regional.stratum_active(s,k))
          //{
            regional.usergridU(s,k) = seasonGridU(s,k);
            regional.usergridV(s,k) = seasonGridV(s,k);
            regional.usergridSigma(s,k) = seasonGridSigma(s,k);
          //}
       }
    }
  } 
  else
  {
    cerr << "Error: void reset(void) not defined for neural network in tagest.tpl" << endl;
    ad_exit(1);
  }

FUNCTION void compute_obrien_sigma()
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  if (param.isRegional()) 
  {
    par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& regional = (par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)param;
    obrienSigma.initialize();
    const double dx = regional.deltax;
    const double dy = regional.deltay;
    const int nseason = regional.get_nseason();
    const int ngrid = regional.get_ngrid();
    for (int season = 1; season <= nseason; season++)
    {
      for (int grid = 1; grid <= ngrid; grid++)
      {
      //  the numerical dispersion bias; with incorrect (uv) units
      // brienSigma(season, grid) = 0.5 * (dx * sfabs(seasonGridU(season, grid)) + dy * sfabs(seasonGridV(season, grid)));
      /*  from extra_s.cpp
          // 15 = 0.5*30
          correct_sigma = param.get_usergrid_sigma(s,k)
                         + 15.0*(dx*fabs(param.get_usergrid_u(s,k))+
                                 dy*fabs(param.get_usergrid_v(s,k)));
      */
          obrienSigma(season, grid) = seasonGridSigma(season, grid)
                         + 0.5 * (dx * sfabs(seasonGridU(season, grid)*30.0) + dy * sfabs(seasonGridV(season, grid)*30.0));
          ddr(season, grid) = sqrt(obrienSigma(season, grid)/(30.0 * M_PI));

      }
    }
  }

FUNCTION void compute_average_fishing_mortality(year_month& date1, year_month& date2)
  indexed_regional_fishery_record& irfr = *indexed_regional_fishery_record::instance;
  average_fishing_mortality.initialize(); 
  total_fishing_mortality = 0.0;
  total_domain_mortality = 0.0;

  if (irfr.is_normalized() != 1)
  {
    return;
  }
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  
  dvector TotalEffortStrata(1,nfleet);
  TotalEffortStrata.initialize();
  /* 
  // never used
  dvector EffortMonths(1,nfleet);
  EffortMonths.initialize();
  double total_domain_cells = 0.0;
  for (int i = 1; i <= param.m; i++)
     for (int j = 1; j <= param.n; j++)
        if (param.gridmap(i,j) > 0)
           total_domain_cells ++;
  */

  for (year_month date = date1; date <= date2; date++)
  {
     get_effort_array(param, irfr, effort, date, effort_occured);
     for (int fleet = 1; fleet <= nfleet; fleet++)
     {
        dmatrix FleetEffort = effort(fleet);
        for (int i = FleetEffort.rowmin(); i <= FleetEffort.rowmax(); i++)
        {
           for (int j = FleetEffort(i).indexmin(); j <= FleetEffort(i).indexmax(); j++)
           {
              if (FleetEffort(i,j) > 0)
                 TotalEffortStrata(fleet) ++;
           }
        }
        //if (TotalEffortStrata(fleet) > 0)
        //  EffortMonths(fleet) += 1.0;
     } 
  }

  const double SumEffortStrata = sum(TotalEffortStrata);
  for (int fleet = 1; fleet <= nfleet; fleet++)
  {
     clogf << fleet << " " << q(fleet) << " " << TotalEffortStrata(fleet) << endl;
     average_fishing_mortality(fleet) = q(fleet)*TotalEffortStrata(fleet)/SumEffortStrata;
  }

  TRACE(TotalEffortStrata)
  TRACE(SumEffortStrata)
  //TRACE(average_fishing_mortality)
  //TRACE(total_domain_cells)

  total_fishing_mortality = sum(average_fishing_mortality);
  total_domain_mortality = mort + total_fishing_mortality;

FUNCTION void compute_likelihood(dvariable& like)
    par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
   if (param.m_ipar(11) != 2)
   {
     cerr << "Unknown likelihood function specified in void computeLikelihood(...)" 
          << "; m_ipar(11) = " << param.m_ipar(11)  << endl;
     exit(1);
   }

   for (int f = fleet0; f <= nfleet; f++)
   {
      for (int i = 1; i <= m; i++)
      {
         int lb = jlb(i);
         int ub = jub(i);
         for (int j = lb; j <= ub; j++)
         {
            double tobs = obs_recaps(f,i,j);
	    dvariable tpred = PredRecaps(f,i,j);
            if ((tobs > 0.0) || (tpred > 0.0))
            {
               residuals(i,j,1) += tobs;
               residuals(i,j,2) += value(tpred);
            }
            // Poisson liklihood is zero if predicted recaptures is zero
            // 0^x = 0; log(0) = NaN
	    if (tpred > 0.0)
	    {
               // negtaive log likelihood
               dvariable temp = -(tobs * log(tpred) - tpred - FACT.log_factorial((int) tobs));
               if (temp < maximum_like_cont)
               {  
                  like -= temp;
               }
	    }
	 } // j loop
       } // i loop
   } // fleet loop
 

REPORT_SECTION
  #undef REPORT
  #define REPORT(object) #object << "\n" << object << '\n'
  clogf << "in report" << endl;
  TTRACE(ifn,iexit)
  TRACE(current_phase())
  TTRACE(last_phase(),sd_phase())
  report << banner << clogf; 

  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  const int nvar =  initial_params::nvarcalc();
  compute_obrien_sigma();
  year_month final_date = param.get_tr_date(param.nrelease) + param.nmonth;
  compute_average_fishing_mortality(param.start_date,final_date);
  _global_report_flag = 1;

  time(&end_time);
  c_end_time = clock();
  double total_elapsed_time = (double)(c_end_time-c_start_time)/(double)CLOCKS_PER_SEC;
  clogf << "\nfinished time: " << ctime(&end_time) << endl;
  clogf << "Total time for estimation: " << total_elapsed_time << " seconds." << endl;

  report << "\nFinished time: " << ctime(&end_time)
         << "Total time for estimation: " << total_elapsed_time << " seconds." << endl
         << "Total time for estimation: " << (total_elapsed_time/60.0) << " minutes." << endl;
  report << REPORT(nvar)
         << REPORT(likelihood)
         << REPORT(likelihood.gmax)
         << REPORT(mort)
         << REPORT(q)
         << REPORT(obrienSigma)
         << REPORT(average_fishing_mortality)
         << REPORT(total_fishing_mortality)
         << REPORT(total_domain_mortality)
         << setprecision(15)
         << endl;
   if (param.m_ipar(55) > 0)      
   {
      report << "\nrecapture_dev:" << endl;
      report << "  n    date          recapture_dev                  edp" << endl; 
      for (int i = recapture_dev.indexmin(); i <= recapture_dev.indexmax(); i++)
      {
          year_month rdate=param.global_start_date+i;
          report << setw(3) << i << " " << rdate << " " 
                 << setw(22) << setprecision(16)  << recapture_dev(i)
                 << setw(22) << setprecision(16)  << exp(recapture_dev(i)) << endl;
      }
      report << REPORT(norm2(recapture_dev))
             << REPORT(sum(recapture_dev))
             << endl;
 
   }
   if (param.m_ipar(84) >= 4)      
      report << REPORT(nb_par) << endl;
         //if (param.m_ipar(84) == 1)
         //   report << REPORT(sus_spd) << endl;
         if (param.m_ipar(84) == 1)
            report << REPORT(chi) << endl;
         if (param.m_ipar(85) == 1)
            report << REPORT(slope) << endl;
         if (param.m_ipar(85) == 1)
            report << REPORT(inflection) << endl;
         if (param.m_ipar(85) == 1)
            report << REPORT(min_D) << endl;

  report << "average_fishing_mortality: " << endl;
  for (int fleet = fleet0; fleet <= param.nfleet; fleet++)
  {
    report << '\t' << param.fleet_names[fleet] << ": " << average_fishing_mortality[fleet] << endl;
  }

  csvstream attr(adstring(argv[0])+"-attr.csv");
  report << "\nObserved and predicted attrition" << endl;
  attr << "t" << "obs" << "pred" << "obs0" << "obsT" << endl;
  for (attr_index = 0; attr_index <= max_attr_index; attr_index++)
  {
    report << setw(4) << attr_index
           << setw(6) << obs_attr(attr_index,1) 
           << setw(18) << setprecision(5) << pred_attr(attr_index)
           << setw(6) << obs_attr(attr_index,2) 
           << setw(6) << obs_attr(attr_index,3)  << endl;
    attr << attr_index << obs_attr(attr_index,1) << pred_attr(attr_index) 
         << obs_attr(attr_index,2) << obs_attr(attr_index,3) << endl;
  }
  report << setw(4) << "sum" << setw(6) << colsum(obs_attr,1) 
         << setw(18) << setprecision(5) << sum(pred_attr) 
         << setw(6) << colsum(obs_attr,2) 
         << setw(6) << colsum(obs_attr,3) << endl;
  attr.close();

  //recaptype_vector &recaps = *recaptype_vector::instance;

  csvstream hist(adstring(argv[0])+"-hist.csv");

  hist << "t" << "year" << "month" << "obs" << "pred" 
       << "obs0" << "obsT" << "rdev" << endl;
  report << "\nObserved and predicted history" << endl;
  //            0 2006/08     1    1.08e+03   161   162
  report << "   n    date  tobs       tpred tobs0  tobs+tobs0  rdev" << endl;
  year_month date = param.start_date;
  ivector obs_sum(1,3);
  obs_sum.initialize();
  for (hist_index = 0; hist_index <= max_hist_index; hist_index++)
  {
    report << setw(4) << hist_index
           << setw(8) << date
           << setw(6) << obs_hist(hist_index,1) 
           << setw(12) << pred_hist(hist_index) 
           << setw(6) << obs_hist(hist_index,2)
           << setw(6) << obs_hist(hist_index,3) 
           << setw(12) << recapture_dev(hist_index) << endl;
    hist << hist_index << date.get_year() << date.get_month_value()
         << obs_hist(hist_index,1) << pred_hist(hist_index) 
         << obs_hist(hist_index,2) << obs_hist(hist_index,3) 
         << recapture_dev(hist_index) << endl;
    date ++;
    obs_sum += obs_hist(hist_index); 
  }
  report << setw(12) << "sum" << setw(6) << obs_sum(1) 
         << setw(12) << setprecision(3) << sum(pred_hist)
         << setw(6) << obs_sum(2) << setw(6) <<  obs_sum(3) << endl;
  report << setw(12) << "sum" << setw(6) << colsum(obs_hist,1)
         << setw(12) << setprecision(3) << sum(pred_hist)
         << setw(6) << colsum(obs_hist,2)<< setw(6) << colsum(obs_hist,3) << endl;

  hist.close();

  csvstream resid(adstring(argv[0])+"-resid.xyz");
  const double dx = 0.5*param.deltax/60.0;
  const double dy = 0.5*param.deltay/60.0;
  resid << "lon" << "lat" << "obs" << "pred" << endl;
  for (int i = residuals.slicemin(); i <= residuals.slicemax(); i++)
  {
     const int j1 = residuals(i).rowmin();
     const int j2 = residuals(i).rowmax();
     const double lon = param.index_to_long(i) + dx;
     for (int j = j1; j <= j2; j++)
     {
        if( (residuals(i,j,1) > 0.0) || (residuals(i,j,2) > 0.0) )
        {
           const double lat = param.index_to_lat(j) + dy;
           resid << lon << lat
                 << residuals(i,j,1) << residuals(i,j,2) << endl;
        }
     }
   }
   resid.close();

    int k = 1;
    int cc = param.tr(k).cohort;
    while (k < param.nrelease)
    {
       double rels = 0;
       while (param.tr(k).cohort == cc)
       {
          rels += param.tr(k).tags;
          k++;
       }
       TTRACE(cc,rels)
       report << param.tr(k).date << setw(6) << rels
          << setw(12) << tag_surv_prop(cc-1) << endl;
       cc = param.tr(k).cohort;
    }


  param.m_ipar[9] = int(total_elapsed_time/60.0);
  if (param.m_ipar[1] > 1) 
  {
    param.savefit(value(likelihood), nvar, likelihood.gmax, 0);
  }
  
TOP_OF_MAIN_SECTION
  pad(); // TOP_OF_MAIN_SECTION
  time(&start_time); // = clock();
  c_start_time = clock();
  fcomp2 = clock();
  ifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_nam.tmp in main()" << endl;
    exit(1);
  }
  char filename[20];
  fnt >> filename;
  fnt.close();

  int last_ext_number(const char* root, const char letter);
  int ext_no = last_ext_number(filename, 'p') + 1;

  char ext[6];
  void make_ext(const char c, const int n, char * ext);

  make_ext('p', ext_no, ext);

  ext[0] = '-';
  strcat(filename, ext);
  #ifndef USE_ADMPI
  argv[0] = filename;
  #endif


  
  //arrmblsize=10000000;
  //gradient_structure::set_CMPDIF_BUFFER_SIZE(1500000000L);
  //gradient_structure::set_GRADSTACK_BUFFER_SIZE(50000000L);
  //gradient_structure::set_MAX_NVAR_OFFSET(1000);
    TRACE(__x86_64)
  #ifdef __x86_64
    // 64 bit configuration
    #ifdef USE_ADMPI
      arrmblsize= 10000000;
      gradient_structure::set_CMPDIF_BUFFER_SIZE(  150000000L);
      gradient_structure::set_GRADSTACK_BUFFER_SIZE(12550000L);
      gradient_structure::set_MAX_NVAR_OFFSET(1000);
    #else
      arrmblsize= 10000000;
      gradient_structure::set_CMPDIF_BUFFER_SIZE( 1500000000L);
      gradient_structure::set_GRADSTACK_BUFFER_SIZE(50000000L);
      gradient_structure::set_MAX_NVAR_OFFSET(1000);
    #endif
  #else
    // 32 bit configuration
    arrmblsize=  1000000;
    gradient_structure::set_CMPDIF_BUFFER_SIZE(  150000000L);
    gradient_structure::set_GRADSTACK_BUFFER_SIZE( 5000000L);
    gradient_structure::set_MAX_NVAR_OFFSET(1000);
  #endif
  
  banner = make_banner("tagest: tag movement estimation procedure; ADMB implementation.");
  cout << banner << endl;

  adstring logname(adstring(argv[0]) + "-tagest.log");
  clogf.open(logname);
  if ( !clogf ) {
    cerr << "Cannot open file " << logname << endl;
    //exit(1);
  }
  cout << "Opened " << logname << endl;
  clogf << banner << endl;

  clogf << "starting time: " << ctime(&start_time) << endl;
  TRACE((double)CLOCKS_PER_SEC)
  TRACE(__cplusplus)

  // posix:
  TTRACE(sysconf(_SC_NPROCESSORS_CONF),sysconf(_SC_NPROCESSORS_ONLN))

  // gnu extention:
  //TTRACE(get_nprocs_conf(),get_nprocs())

  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW ); 


FINAL_SECTION
  recaptype_vector::deleteInstance();
  indexed_regional_fishery_record::deleteInstance();
  coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::deleteInstance();
  delete par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance = 0;

  clogf.close();
