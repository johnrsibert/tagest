//$Id: par2xml.tpl 2957 2012-03-14 02:03:13Z jsibert $

  /// \file par2xml.tpl ADMB template file for converting par files to xml files
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
  #include "ADMB_XMLDoc.h"
  #include "MM_XMLDoc.h"
  adstring make_banner(const adstring& program_name);

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

  int last_ext_number(const char* root, const char letter);
  void make_ext(const char c, const int n, char * ext);

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
    const int phaseSus_spd = param.m_ipar(86) == 1 && param.m_ipar(84) == 1 ? 1 : -1;
    //const int phaseMin_D = param.m_ipar(87) == 1 && param.m_ipar(85) == 1 ? 1 : -1;
    //const int phaseSlope = param.m_ipar(88) == 1 && param.m_ipar(85) == 1 ? 1 : -1;
    //const int phaseInflection = param.m_ipar(89) == 1 && param.m_ipar(85) == 1 ? 1 : -1;
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
  !!  cout  << "phaseSigma2: " << phaseSigma << endl;
  init_bounded_number_matrix seasonGridSigma(1,nseason,1,ngrid,minsig,maxsig,phaseSigma);


  !! if (param.m_ipar(84) == 1)
  !! {
       init_bounded_number sus_spd(param.minsus_spd,param.maxsus_spd,phaseSus_spd);
  !!   sus_spd = value(regional.get_sus_spd());
  !! }
  !!  ivector phaseFAD(1,ngrid);
  !!  phaseFAD = -1;
  !!  if (param.m_ipar(85) == 1) 
  !!  {
  !!    for (int f = 1; f <= ngrid; f++)
  !!    {
  !!       phaseFAD(f) = regional.fadmap_stratum_active_d(f) == 1 ? 1 : -1;
  !!    }
  !!    cout << "maxinflection: " << param.maxinflection << endl;
  !!    const double mininflection=0.0;
        //init_bounded_number_vector min_D(1,ngrid,param.minMin_D,param.maxMin_D,phaseFAD);
        init_bounded_number_vector slope(1,ngrid,param.minslope,param.maxslope,phaseFAD);
        init_bounded_number_vector inflection(1,ngrid,mininflection,param.maxinflection,phaseFAD);
  !!    cout << "mininflection: " << mininflection << endl;  
  !!    cout << "maxinflection: " << param.maxinflection << endl;  
  !!    for (int f = 1; f <= ngrid; f++)
  !!    {
  //!!      min_D(f) = value(regional.get_min_D()(f));
  !!      slope(f) = value(regional.get_slope()(f));
  !!      inflection(f) = value(regional.get_inflection()(f));
  !!      cout << "slope: " << slope << endl;  
  !!      cout << "inflection: " << inflection << endl;  
  !!    }
  !!  }
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
   if (coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::createInstance(*par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance) == 0) 
   {
   }
   param.set(*this);


PROCEDURE_SECTION
  usefun1 = clock();
  fcomp1 = fcomp2;
  fcomp2 = clock();
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* param = par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  saveXMLFile(param);
  ad_exit(2);


FUNCTION void saveXMLFile(par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>* param)
  par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& regional = (par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>&)*param;
  ostringstream pppf;
  const int nvar =  initial_params::nvarcalc();
  pppf << "p" << setw(2) << setfill('0') << hex << param->m_ipar[8];
  ostringstream xmlf;
  xmlf << param->pathname.getRoot() << ".x" << setw(2) << setfill('0') << hex << param->m_ipar[8];

  //MM_XMLDoc xml("FIT","par2xml",adstring(pppf.str().c_str()), adstring(xmlf.str().c_str()));
  ADMB_XMLDoc xml("FIT","par2xml",adstring(pppf.str().c_str()), adstring(xmlf.str().c_str()));

  double test = 12345.671890123456;
  int ret = 0;
  ret = xml.createXMLelement("test",test, "Dummy double");
  TRACE(ret)
  likelihood = param->objective_function_value;
  likelihood.gmax = param->maximum_gradient;
  ret = xml.createXMLcomment("fit statistics");
  ret = xml.createXMLelement(likelihood);
  ret = xml.createXMLcomment("parameters of regional model");
  ret = xml.createXMLelement("q",q,"Catchability Coefficient");
  TRACE(ret)
  ret = xml.createXMLelement(mort,"Natural Mortality (per month)");
  TRACE(ret)
  ret = xml.createXMLelement(special_mort,"Region-specific Natural Mortality (per month)");
  TRACE(ret)
  ret = xml.createXMLelement(report_rates,"Tag Reporting Rate");
  TRACE(ret)
  ret = xml.createXMLelement(nb_par,"Negative Binomial Parameter");
  TRACE(ret)
  ret = xml.createXMLelement(param->startmonth,"Months to start each season");
  TRACE(ret)

  /*
  TRACE(ret)
  ret = xml.createXMLelement(seasonGridU,"West to East component of directed movement (Nm per day)");
  TRACE(ret)
  ret = xml.createXMLelement(seasonGridV,"South to North component of difected movement (Nm per day)");
  TRACE(ret)
  ret = xml.createXMLelement(seasonGridSigma,"Dispersive movement (sq Nm per month)");
  TRACE(ret)
  */
  xml.createXMLelement("nvar",nvar,"Number of parameters");
  xml.createXMLelement("likelihood",value(likelihood),"Value of likelihood function");
  xml.createXMLelement("likelihood.gmax",likelihood.gmax,"Maximum gradient contribution");

  /*
  for (int i=0;i< initial_params::num_initial_params;i++)
  {
     //(varsptr[i])->save_value();
     xml.createXMLelement(*initial_params::varsptr[i]);

  }
  */
  ret = xml.createXMLcomment("determinants of model domain");
  ret = xml.createXMLelement("m",param->m,"Number of cells in x dimension");
  ret = xml.createXMLelement("n",param->n,"Number of cells in y dimension");
  ret = xml.createXMLelement("deltax",param->deltax,"Step size in x dimension (Nm)");
  ret = xml.createXMLelement("deltay",param->deltay,"Step size in y dimension (Nm)");

  ostringstream lonstr;
  lonstr << param->sw_coord.get_long();
  adstring adlonstr(lonstr.str().c_str());
  ret = xml.createXMLelement("swlon",adlonstr, "Longitde of southwest corner of model domain");

  ostringstream latstr;
  latstr << param->sw_coord.get_lat();
  adstring adlatstr(latstr.str().c_str());
  ret = xml.createXMLelement("swlat",adlatstr, "Latitude of southwest corner of model domain");
  ret = xml.createXMLelement("west_bndry",param->west_bndry,"Boundary condition for west boundary");
  ret = xml.createXMLelement("east_bndry",param->east_bndry,"Boundary condition for east boundary");
  ret = xml.createXMLelement("north_bndry",param->north_bndry,"Boundary condition for north boundary");
  ret = xml.createXMLelement("south_bndry",param->south_bndry,"Boundary condition for south boundary");

  xml.createXMLelement("nmonth",param->nmonth,"Number of months to follow cohort attrition");
  int StartYear = param->start_date.get_year();
  xml.createXMLelement("start_yr",StartYear,"Year of first tag release");
  int StartMonth =  param->start_date.get_month_value();
  xml.createXMLelement("start_mo",StartMonth,"Month in year of first tag release");
  xml.createXMLelement("nfleet",param->nfleet,"Number of fishing fleets to use in the model");
  //xml.createXMLelement("fleet_names",param->fleet_names,"Names of fleets to use in the model");
  //xml.createXMLelement("DomesticFleetKludgeFlag",param->DomesticFleetKludgeFlag,"Flag to set effort according to recaptures");
  xml.createXMLelement("nrelease",param->nrelease,"Number of tag release cohorts to use in the model");

  ret = xml.createXMLcomment("                     ");
  ret = xml.createXMLcomment("END OF TESTING SECION");
  TRACE(ret)
  ret = xml.write();
  TRACE(ret)
  #undef START_TEST
  #ifdef START_TEST

  xml.createXMLelement("trv_ptr", param->tr, param->nrelease, "Pooled tag release cohorts");

  xml.createXMLelement("ngrid",regional.get_ngrid(),"Number of regions in model domain");
  xml.createXMLelement("nseason",regional.get_nseason(),"Number of seasons in model");

  xml.createXMLelement("minsig",param->minsig,"Minimum bound on diffusivity estimate");
  xml.createXMLelement("maxsig",param->maxsig,"Maximum bound on diffusivity estimate");
  xml.createXMLelement("minmort",param->minmort,"Minimum bound on natural mortality estimate");
  xml.createXMLelement("maxmort",param->maxmort,"Maximum bound on natural mortality estimate");
  xml.createXMLelement("minq",param->minq,"Minimum bound on catchability estimate");
  xml.createXMLelement("maxq",param->maxq,"Maximum bound on catchability estimate");
  xml.createXMLelement("minr_rates",param->minr_rates,"Minimum bound on reporting rate estimate");
  xml.createXMLelement("maxr_rates",param->maxr_rates,"Maximum bound on reporting rate estimate");
  xml.createXMLelement("minvel",param->minvel,"Minimum bound on directed movement estimate");
  xml.createXMLelement("maxvel",param->maxvel,"Maximum bound on directed movement estimate");

  xml.createXMLelement("m_ipar",param->m_ipar,"The ipars");
  ret = xml.createXMLelement("gridmap",param->gridmap,"Land mask");
  #endif // START_TEST

TOP_OF_MAIN_SECTION
  ifstream fnt("file_nam.tmp");
  if ( !fnt)
  {
    cerr << "Cannot open file_nam.tmp in main()" << endl;
    ad_exit(1);
  }
  char filename[20];
  fnt >> filename;
  fnt.close();


  
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
  
  adstring banner = make_banner("par2xml: movemod par file converter.");
  cout << banner << endl;

  adstring logname("par2xml.log");
  clogf.open(logname);
  if ( !clogf ) 
  {
    cerr << "Cannot open file " << logname << endl;
    //exit(1);
  }
  cout << "Opened " << logname << endl;
  clogf << banner << endl;

FINAL_SECTION
  recaptype_vector::deleteInstance();
  indexed_regional_fishery_record::deleteInstance();
  coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::deleteInstance();
  delete par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance;
  par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::instance = 0;

  clogf.close();
