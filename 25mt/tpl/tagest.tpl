//$Id: tagest.tpl 2985 2012-07-12 01:22:24Z jsibert $

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
  //#include <sys/sysinfo.h>

  setsaveinterflag interflag;
  int saveinterflag = MM_intersave::ADMB_mode;

  int _global_report_flag = 0;

  ofstream clogf;

  // some timers
  clock_t start_time, end_time;
  clock_t usefun1, usefun2;
  clock_t fcomp1, fcomp2; 

  #ifdef __CYGWIN32__
  void ad_comm::get_slave_assignments() {;} // does nothing; satisfies reference
  #endif

  template class par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
  template class par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
  template class par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>;

  template <typename MATRIX1, typename MATRIX2> 
  void resid_comp(const MATRIX1& obs, const MATRIX2& pred, const MATRIX1& like, const int report);

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

DATA_SECTION
  !!  ad_comm::change_datafile_name("file_nam.tmp");
  init_adstring filename;

  int m;
  int m1;
  int n;
  int nfleet;

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

  3darray Recaps1;
  3darray Recaps0;

PARAMETER_SECTION
 LOCAL_CALCS
    if (par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::createInstance(filename) == 0) 
    {
    }

    par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
    m = param.get_m();
    n = param.get_n();
    nfleet = param.get_nfleet();

    // set some parameters to control minimizer display
    maxfn  = param.m_ipar(1);
    iprint = param.m_ipar(2);
    crit   = param.m_ipar(3)/100.;
    imax   = param.m_ipar(4);
    scroll_flag = param.m_ipar(7);
    ifn    = 0;

    param.executable = adstring(argv[0]);
    //const int phaseQ = param.m_ipar(25) == 2 ? 1 : -1;// won't work for uniform q
    ivector phaseQ(1,nfleet);
    phaseQ = -1;
    if (param.m_ipar(25) == 2)
    {
      for (int f = 1; f <= nfleet; f++)
        phaseQ(f) = 1;
    }
    else if (param.m_ipar(25) == 1)
    {
      phaseQ(1) = 1;
      for (int f = 2; f <= nfleet; f++)
        phaseQ(f) = -1;
    }
    else if (param.m_ipar(25) > 2)
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

  init_bounded_number_vector q(1,nfleet,param.minq,param.maxq,phaseQ);
  init_bounded_number mort(param.minmort,param.maxmort,phaseMort);
  init_bounded_number special_mort(param.minmort,param.maxmort,phaseSpecial_mort_phase);
  init_bounded_vector report_rates(1,nfleet,param.minr_rates,param.maxr_rates,phaseR_rates);

  sdreport_vector average_fishing_mortality(1,nfleet);
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
  !!   const extern double min_nb_par;
  !!   const extern double max_nb_par;
  init_bounded_vector nb_par(1,nfleet,min_nb_par,max_nb_par,phaseNb_par);

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
  !! 
  !!  cout  << "phaseUV: " << phaseUV << endl;
  //!! exit(1);
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

  
  //init_bounded_matrix seasonGridU(1,nseason,1,ngrid,param.minvel,param.maxvel,phaseUV);
  //init_bounded_matrix seasonGridV(1,nseason,1,ngrid,param.minvel,param.maxvel,phaseUV);
  //init_bounded_matrix seasonGridSigma(1,nseason,1,ngrid,param.minsig,param.maxsig,phaseSigma);

  !!  if (param.m_ipar(84) == 1)
  !!  {
  !!    const int phaseSus_spd = param.m_ipar(86) == 1 ? 1 : -1;
        init_bounded_number sus_spd(param.minsus_spd,param.maxsus_spd,phaseSus_spd);
  !!    sus_spd = value(regional.get_sus_spd());
  !!  }
  !!  if (param.m_ipar(85) == 1) 
  !!  {
  //!!     ivector phaseFAD(1,ngrid);
  //!!     phaseFAD = -1;
  //!!     for (int f = 1; f <= ngrid; f++)
  //!!     {
  //!!        phaseFAD(f) = regional.fadmap_stratum_active_d(f); // == 1 ? 1 : -1; 
  //!!     }
  !!     ivector phaseSlope (1,ngrid);
  !!     phaseSlope = param.m_ipar(88) == 1 ? 1 : -1;
  !!     ivector phaseInflection (1,ngrid);
  !!     phaseInflection = param.m_ipar(89) == 1 ? 1 : -1;
  !!     const double mininflection=0.0;
  !!     cout << "mininflection: " << mininflection << endl;  
  !!     cout << "maxinflection: " << param.maxinflection << endl;  
  !!     for (int f = 1; f <= ngrid; f++)
  !!     {
  !!        if (param.m_ipar(88) == 1 && regional.fadmap_stratum_active_d(f) >= 10)
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
  !!        if (param.m_ipar(89) == 1 && (regional.fadmap_stratum_active_d(f) == 1 || regional.fadmap_stratum_active_d(f) == 11))
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
         init_bounded_number_vector slope(1,ngrid,param.minslope,param.maxslope,phaseSlope);
         init_bounded_number_vector inflection(1,ngrid,mininflection,param.maxinflection,phaseInflection);
  !!     cout << "regional.fadmap_stratum_active_d: " << regional.fadmap_stratum_active_d << endl;
  !!     cout << "phaseSlope: " << phaseSlope << endl;
  !!     cout << "phaseInflection: " << phaseInflection << endl;
  !!     cout << "param.minslope: " << param.minslope << endl;
  !!     cout << "param.maxslope: " << param.maxslope << endl;
  !!     cout << "mininflection: " << mininflection << endl;
  !!     cout << "param.maxinflection: " << param.maxinflection << endl;
  !!     for (int f = 1; f <= ngrid; f++)
  !!     {
  !!       slope(f) = value(regional.get_slope()(f));
  !!       inflection(f) = value(regional.get_inflection()(f));
  !!       cout << "slope: " << slope << endl;  
  !!       cout << "inflection: " << inflection << endl;  
  !!     }
  !!   }
  sdreport_matrix obrienSigma(1,nseason,1,ngrid);
  sdreport_matrix ddr(1,nseason,1,ngrid); ///< daily diffusion radius
  
  //sdreport_number L;
  //sdreport_number U;
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
 END_CALCS

  !! }
  !! else if (param.isNeuralNet()) 
  !! {
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
    if (param.m_ipar(25) == 2)
    {
      for (int f = 1; f <= nfleet; f++)
          q(f) = param.q(f);
    }
    else if  (param.m_ipar(25) == 1)
    {
      q(1) = param.q(1);
      for (int f = 2; f <= nfleet; f++)
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

    Recaps1.allocate(0, nfleet, ilbv, iubv, jlbm, jubm);
    Recaps1.initialize();
    Recaps0.allocate(0, nfleet, ilbv, iubv, jlbm, jubm);
    Recaps0.initialize();


 END_CALCS
  //3darray fishing_mortality(1,nfleet,ilbv,iubv,jlbm,jubm);
  3darray FishMort1(0,nfleet,ilbv,iubv,jlbm,jubm);
  3darray FishMort0(0,nfleet,ilbv,iubv,jlbm,jubm);
  matrix total_mortality(1,m,jlb,jub);

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
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;

  if (indexed_regional_fishery_record::createInstance(filename) == 0) { }
  indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;

  if (recaptype_vector::createInstance(filename, *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance, irfr) == 0) { }
  recaptype_vector &recaps = *recaptype_vector::instance;
  const int nrec = recaps.getTotalRecords();
  if (nrec <= 0) 
  {
    cerr << "Error: No recapture data found." << endl;
  }
  if (param.m_ipar[13] == 2) 
  {
    param.collapse_cohorts(recaps, nrec);
  }

  ivector effortless = recaps.count_effortless_recaps();
  clogf << "Recaptures WITHOUT fishing effort: " << effortless(0) << endl;
  clogf << "Recaptures  with   fishing effort: " << effortless(1) << endl;
  clogf << "Total recaptures:                  " << effortless(2) << endl;

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
   for (int f = 1; f <= nfleet; f++)
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


PROCEDURE_SECTION
  usefun1 = clock();
  fcomp1 = fcomp2;
  fcomp2 = clock();
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* param = par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  recaptype_vector &recaps = *recaptype_vector::instance;
  
  reset();
  //recaps.reset_position();
  //recaps.reset_position();
  dvar_matrix release(1,m,jlb,jub);
  release.initialize();

  //dvar3_array fish_mort = fishing_mortality;
  //dvar_matrix tot_mort = total_mortality;

  likelihood -= param->fpen;

  param->add_total_bounds_penalty(param->fpen);

  year_month save_start_date = param->start_date;

  int ncohort = param->nrelease;
  int cohort = 0;
  int last_rn = 0;
  int c1 = 1;
  int c2 = ncohort;
  int pool_tags = param->m_ipar[13];
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
  }

  indexed_regional_fishery_record &irfr = *indexed_regional_fishery_record::instance;
  coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable> &coff = *coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  const int nrec = recaps.getTotalRecords();

  funnel_dvariable funnel;
  year_month local_start_date = param->start_date;
  year_month final_date = param->get_tr_date(param->nrelease) + param->nmonth;
  TTRACE(param->start_date,final_date)
  for (int cc = c1; cc <= c2; cc++)
  {
    int cohort = 0;
    if (pool_tags == 2)
    {
      cohort = param->get_tr_cohort(cc);
      //clogf << "\nReleasing tags for cohort " << cohort << endl;
      local_start_date = param->get_tr_date(cc);
      final_date = local_start_date + param->nmonth;
      param->start_date = local_start_date;
      release.initialize();
      while ((last_rn < ncohort) && (cohort == param->get_tr_cohort(last_rn+1)))
      {
        last_rn++;
        int i = param->get_tr_i(last_rn);
        int j = param->get_tr_j(last_rn);
        release(i,j) += param->get_tr_tags(last_rn);
        //clogf << local_start_date <<", released "
        //      << param->get_tr_tags(last_rn) << " at " << i << "," << j << endl;
      }  // end while
      cc = last_rn;
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
    if (param->isRegional())
    {
      ad_begin_funnel();
    }

    recaps.reset_position();
    int current_season = 0;
    for (year_month date = local_start_date; date <= final_date; date++) 
    {
      //TRACE(date)
      if (pool_tags == 1)
      {
        while (last_rn < ncohort && date == param->get_tr_date(last_rn+1))
        {
          last_rn++;
          const int i = param->get_tr_i(last_rn);
          const int j = param->get_tr_j(last_rn);
          release(i,j) += param->get_tr_tags(last_rn);
          //clogf << date <<", released " << param->get_tr_tags(last_rn) << " at "
          //      << i << "," << j << endl;
        }
      }
      if (param->isRegional())
      {
        par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& regional = (par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)*param;
        if (current_season != regional.get_season(date))
        {
          current_season = regional.get_season(date);
          regional.uvs_comp(u, v, sigma, date);

          dvariable penlike = 0.0;
          velpen<dvar3_array,dvar_matrix,dvar_vector,dvariable>(regional, penlike, date);
          param->add_total_uv_penalty(penlike);
          plike += penlike;

          penlike = 0.0;
          sigpen(regional, penlike, date);
          param->add_total_sigma_penalty(penlike);
          plike += penlike;

          coff.comp(*param, u, v, sigma);
        }
      }
      else if (param->isNeuralNet())
      {
        par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>& neuralnet = (par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)*param;
        neuralnet.uvs_comp(u, v, sigma, date);
        if (param->m_ipar[33] == 1)
        {
          const double uv_xweight = (const double)param->m_ipar[31]*1e-9;
          const double uv_yweight = (const double)param->m_ipar[32]*1e-9;
          dvariable uv_penalty = 0.0;
          velpen<dvar_matrix,dvariable>(u, v, uv_xweight, uv_yweight, uv_penalty);
          plike -= uv_penalty;
          param->add_total_uv_penalty(uv_penalty);
        }
        if (param->m_ipar[41] == 1) 
        {
          const double sig_weight = (const double)param->m_ipar[42]*1e-9;
          dvariable sigma_penalty = 0.0;
          sigpen<dvar_matrix,dvariable>(sigma, sig_weight, sigma_penalty);
          plike -= sigma_penalty;
          param->add_total_sigma_penalty(sigma_penalty);
        }
        coff.comp(*param, u, v, sigma);
      }
      //likelihood += norm2(sigma);  
      //continue;

      recaps.getreturns(Recaps1, Recaps0, date, cohort);

      get_effort_array(*param, irfr, effort, date, effort_occured);

      //param->fish_mort_comp(fish_mort, effort, date);
      param->fish_mort_comp(FishMort1, FishMort0, effort, date, release, 
                Recaps0, irfr.get_mean_effort() );//mort=q*effort

      param->total_mort_comp(FishMort1, FishMort0, total_mortality);
      
      coff.b_adjust(*param, total_mortality);
    // do not include effortless recaps in likelihood
    #undef USE_EFFORTLESS_RECAPS
    #ifdef USE_EFFORTLESS_RECAPS
      d3_array tRecaps = Recaps1 + Recaps0;
      dvar3_array tFishMort = FishMort1 + FishMort0;
      compute_likelihood(release, date, cohort, tRecaps, nrec, total_mortality, effort_occured, tFishMort, nfleet, plike, *param);
    #else
      compute_likelihood(release, date, cohort, Recaps1, nrec, total_mortality, effort_occured, FishMort1, nfleet, plike, *param);
    #endif
      dvariable neg_tag_pen = 0;
      negative_tag_penalty<dvar3_array,dvar_matrix,dvar_vector,dvariable>(release, *param, neg_tag_pen);
      param->add_total_negative_tag_penalty(neg_tag_pen);
      plike -= neg_tag_pen;

      dvariable tot_tag_pen = 0;
      total_tag_penalty<dvar3_array,dvar_matrix,dvar_vector,dvariable>(release, *param, tot_tag_pen);
      param->add_total_total_tag_penalty(tot_tag_pen);
      plike -= tot_tag_pen;

      coff.adi(release,NULL);
    }
    if (param->isRegional())
    {
      funnel = plike;
      likelihood += funnel;
    }
    else if (param->isNeuralNet())
    {
      likelihood += plike;
    }
  }
  param->start_date = save_start_date;
  likelihood *= -1;

  natural_mortality = mort;
  if (_global_report_flag)
  {
    resid_comp(release, release, release, 1);
  }
  //time_t time_sec;
  //time(&time_sec);
  //clock_t time2 = clock();
  usefun2 = clock();
  double uf_elapsed_time = (double)(usefun2-usefun1)/CLOCKS_PER_SEC;
  double of_elapsed_time = (double)(fcomp2-fcomp1)/CLOCKS_PER_SEC;
  /*
  double elapsed_time = (double)(time2-time1)/CLOCKS_PER_SEC;
  clogf << "Objective function value = " << setprecision(8) << plike
        << "; elapsed time = " << setprecision(2) << elapsed_time 
        << " seconds." << endl;
  */

  clogf << "Objective function value = " << setprecision(8) << likelihood
        << "; elapsed time = " << setprecision(2) << of_elapsed_time 
        << " seconds. User function time = " << setprecision(2) 
        << uf_elapsed_time << " seconds." << endl;

 
  /*
  if(1)
  {
    TTRACE(param->start_date,final_date)
    compute_average_fishing_mortality(param->start_date,final_date);
    //exit(1);
  }
  */
  
  if (sd_phase())
  {
    compute_obrien_sigma();
    compute_average_fishing_mortality(param->start_date,final_date);
  }

  if ( ((++param->m_ipar(5) % 10) == 0) && !sd_phase())
  {
    const int nvar =  initial_params::nvarcalc();
    param->savefit(value(likelihood), nvar, likelihood.gmax, ".tmp");
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
    for (int f = 1; f <= nfleet; f++)
      param.q(f) = q(f);
  }
  else if  (param.m_ipar(25) == 1)
  {
    param.q(1) = q(1);
    for (int f = 2; f <= nfleet; f++)
      param.q(f) = q(1);
  }

  param.mort = mort;

  param.special_mort = special_mort;
  param.report_rates = report_rates;
  param.nb_par = nb_par;
  param.sus_spd = sus_spd;

  /*
  if(param.m_ipar(25) == 1)
  {
     //uniform q
     param.q(1) = q(1);
     for (int f = 2; f <= param.nfleet; f++)
     {
        param.q(f) = q(1);
     }
   }
  else if(param.m_ipar(25) == 2)
  {
      //separate q for each fleet
      param.q = q;
  }
  */

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
               regional.slope(k) = slope(k);
               regional.inflection(k) = inflection(k);
               TRACE(slope)
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
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* param = par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  if (param->isRegional()) 
  {
    par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& regional = (par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)*param;
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
  dvector EffortMonths(1,nfleet);
  EffortMonths.initialize();
  double total_domain_cells = 0.0;
  for (int i = 1; i <= param.m; i++)
     for (int j = 1; j <= param.n; j++)
        if (param.gridmap(i,j) > 0)
           total_domain_cells ++;


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
        if (TotalEffortStrata(fleet) > 0)
           EffortMonths(fleet) += 1.0;
     } 
  }

  double SumEffortStrata = sum(TotalEffortStrata);
  for (int fleet = 1; fleet <= nfleet; fleet++)
  {
     clogf << fleet << " " << q(fleet) << " " << TotalEffortStrata(fleet) << endl;
     average_fishing_mortality(fleet) = q(fleet)*TotalEffortStrata(fleet)/SumEffortStrata;
  }

  //TRACE(TotalEffortStrata)
  //TRACE(SumEffortStrata)
  //TRACE(average_fishing_mortality)
  //TRACE(total_domain_cells)

  total_fishing_mortality = sum(average_fishing_mortality);
  total_domain_mortality = mort + total_fishing_mortality;

FUNCTION void compute_likelihood_log_normal(dvariable& like, dvar_matrix& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dvar_matrix& z, ivector& effort_occured, dvar3_array& fmort)
    void getreturns(dmatrix& returns, const int fleet, const year_month& date, const int cohort, const recaptype_vector& recaps, const int nrec, const int m, const int n);
    int _m = param.get_m();
    int _n = param.get_n();
    ivector& jlb = param.get_jlb();
    ivector& jub = param.get_jub();

    dmatrix obs(1, _m, jlb, jub);
    dvar_matrix pred(1, _m, jlb, jub);

    int nf = param.get_nfleet();
    for (int f = 1; f <= nf; f++)
    {
        //if (effort_occured(f))
        {
           getreturns(obs, f, date, cohort, recaps, nrec, _m, _n);
 
           param.pred_recapture_comp(pred, pred_tags, z, fmort, f, date);

           int count = 0;
           for (int i=1; i <=_m; i++)
           {
               int lb = jlb(i);
               int ub = jub(i);
               for (int j=lb; j <= ub; j++)
               {
                   double tobs  = obs(i,j);
                   dvariable tpred = pred(i,j);
                   count ++;
                   dvariable temp = pow((log(tobs+1)-log(tpred+1)),2);
                   like-=temp;
               }//for j
           }//for i
        }//if
    }//for f 

FUNCTION void compute_likelihood(dvar_matrix& release, year_month& date, const int cohort, d3_array& recaps, const int nrec, dvar_matrix& tot_mort, ivector& effort_occured, dvar3_array& fish_mort, const int nfleet, dvariable& plike, par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param)
  dvariable plike1 = 0.0;
  switch (param.m_ipar[11])
  {
    case 2:
      if (param.m_ipar[27] == 0 || param.m_ipar[27] == 1)
      {
        poisson_like<dvar3_array,dvar_matrix,dvar_vector,dvariable>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort);
      }
      //else if (param.ipar[27] == 2)
      //{
      //poisson_like(plike1, psi_bar, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort);
      //}
      else
      {
        cerr << "Error: Unknown option (" << param.m_ipar[27] << ") passed to fgcomp in " << __FILE__ << endl;
        exit(1);
      }
      break;

   case 3:
     //{
     //least_square_like(plike1, release, date, cohort, recaps, nrec, param, effort, fish_mort);
     //}
     cerr << "switch[11] = 3; " << "least squares likelihood not implemented" << endl;
     exit(1);
     break;

   case 4:
   case 5:
     //negative_binomial_like<dvar3_array,dvar_matrix,dvar_vector,dvariable>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort,param.nb_par,1,param.m_ipar[18]);
     //break;
   case 14:
   case 15:
     //negative_binomial_like<dvar3_array,dvar_matrix,dvar_vector,dvariable>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort,param.nb_par,2,param.m_ipar[18]);
     cerr << "liklihood options 4, 5, 14, and 15 no longer supported" << endl;
     cerr << "you had m_ipar[11] = " << param.m_ipar[11] << endl;
     exit(1);
     break;
   case 24:
   case 25:
     negative_binomial_like<dvar3_array,dvar_matrix,dvar_vector,dvariable>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort,param.nb_par,nfleet,param.m_ipar[18]);
     break;
   /*
   case 6:
     exponential_like<dvar3_array,dvar_matrix,dvar_vector,dvariable>(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort);
     break;
   case 7:
     compute_likelihood_log_normal(plike1, release, date, cohort, recaps, nrec, param, tot_mort, effort_occured, fish_mort);
     break;
   */
   default:
     cerr << "Unknown likelihood function specified in void computeLikelihood(MATRIX& release,..." << "; param.m_ipar[11] = " << param.m_ipar[11]  << endl;
     exit(1);
  } // switch (param.m_ipar[11])
  plike += plike1;

REPORT_SECTION
  #undef REPORT
  #define REPORT(object) #object << "\n" << object << '\n'

  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param = *par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  const int nvar =  initial_params::nvarcalc();

  end_time = clock();
  double total_elapsed_time = (double)(end_time-start_time)/CLOCKS_PER_SEC;
  clogf << "\nfinished time: " << ctime(&end_time) << endl;
  clogf << "Total time for estimation: " << total_elapsed_time << " seconds." << endl;
  clogf << "Total time for estimation: " << (total_elapsed_time/60.0) << " minutes." << endl;

  report << "\nFinished time: " << ctime(&end_time)
         << "Total time for estimation: " << total_elapsed_time << " seconds." << endl
         << "Total time for estimation: " << (total_elapsed_time/60.0) << " minutes." << endl;
  report << REPORT(nvar)
         << REPORT(mort)
         << REPORT(q)
         << REPORT(obrienSigma)
         << REPORT(average_fishing_mortality)
         << REPORT(total_fishing_mortality)
         << REPORT(total_domain_mortality)
         << REPORT(nb_par)
         << setprecision(15)
         << REPORT(likelihood)
         << REPORT(likelihood.gmax)
         << endl;
         if (param.m_ipar(84) == 1)
            report << REPORT(sus_spd) << endl;
         if (param.m_ipar(85) == 1)
            report << REPORT(slope) << endl;
         if (param.m_ipar(85) == 1)
            report << REPORT(inflection) << endl;

  report << "average_fishing_mortality: " << endl;
  for (int fleet = 1; fleet <= param.nfleet; fleet++)
  {
    report << '\t' << param.fleet_names[fleet] << ": " << average_fishing_mortality[fleet] << endl;
  }

  param.m_ipar[9] = int(total_elapsed_time/60.0);
  if (param.m_ipar[1] > 1) 
  {
    param.savefit(value(likelihood), nvar, likelihood.gmax, 0);
  }
  
  //time_t time2 = time(NULL);
  //const double elapsed_time = (double)(time2-time1);
  //clogf << "Total time for estimation: " << elapsed_time/60.0 << " minutes" << endl;
  //report << "Total time for estimation: " << elapsed_time/60.0 << " minutes" << endl;

TOP_OF_MAIN_SECTION
  start_time = clock();
  fcomp2 = start_time;
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

  char ext[5];
  void make_ext(const char c, const int n, char * ext);

  make_ext('p', ext_no, ext);

  ext[0] = '-';
  strcat(filename, ext);
  #ifndef USE_ADMPI
  argv[0] = filename;
  #endif

  adstring make_banner(const adstring& program_name);

  
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
  
  adstring banner = make_banner("tagest: tag movement estimation procedure; ADMB implementation.");
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

  // posix:
  TTRACE(sysconf(_SC_NPROCESSORS_CONF),sysconf(_SC_NPROCESSORS_ONLN))

  // gnu extention:
  //TTRACE(get_nprocs_conf(),get_nprocs())




FINAL_SECTION
  recaptype_vector::deleteInstance();
  indexed_regional_fishery_record::deleteInstance();
  coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::deleteInstance();
  delete par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance = 0;

  clogf.close();
