#include "par_t_nn.h"
#include "precomp.h"
#include "intersav.h"
#include "fish_rec.h"
#include <iomanip.h>
#include "trace.h"

#ifdef __GNUC__
#include <iostream.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef __SUN__
#include <unistd.h>
#include <fcntl.h>
#endif


#if !defined(__SUN__) && !defined(__GNUC__)
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#endif

void resid_comp(const dmatrix& obs, const dmatrix& pred, const dmatrix& like,
                const int report);
//void velpen(par_t_reg& param, double& temp, year_month& date);
//void dfvelpen(par_t_reg& dfparam, par_t_reg& param, year_month& date);
//void sigpen(par_t_reg& param, double& temp, year_month& date);
//void dfsigpen(par_t_reg& dfparam, par_t_reg& param, year_month& date);

void total_tag_penalty(dmatrix& tags, par_t& param, double& pen);
void df_total_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t& param,
       double& dfpen);

void negative_tag_penalty(dmatrix& tags, par_t& param, double& pen,
       int& negatives);
void df_negative_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t& param,
       double& dfpen);

void df_poisson_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags,
       year_month& date, int cohort,
       recaptype_vector& recaps, int nrec, par_t& param, par_t& dfparam,
       dmatrix& z, dmatrix& dfz,
       ivector& effort_occured, d3_array& fmort, d3_array& dffmort);

void poisson_like(double& like, dmatrix& pred_tags, year_month& date,
       int cohort, recaptype_vector& recaps, int nrec, par_t& param,
       dmatrix& z, ivector& effort_occured, d3_array& fmort);

void df_nb_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags,
             year_month& date, int cohort,
             recaptype_vector& recaps, int nrec, par_t& param, par_t& dfparam,
             dmatrix& z, dmatrix& dfz,
             ivector& effort_occured, d3_array& fmort, d3_array& dffmort,
             dvector& a, dvector& dfa, const int naa, const int nb_scale);

void negative_binomial_like(double& like, dmatrix& pred_tags, year_month& date, int cohort,
             recaptype_vector& recaps, int nrec, par_t& param, dmatrix& z,
             ivector& effort_occured, d3_array& fmort, dvector& a,
             const int naa, const int nb_scale);

void exponential_like(double& like, dmatrix& pred_tags, year_month& date, int cohort,
       recaptype_vector& recaps, int nrec, par_t& param, dmatrix& z,
       ivector& effort_occured, d3_array& fmort);
void df_exp_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags,
             year_month& date, int cohort,
             recaptype_vector& recaps, int nrec, par_t& param, par_t& dfparam,
             dmatrix& z, dmatrix& dfz,
             ivector& effort_occured, d3_array& fmort, d3_array& dffmort);


//void sigpen(par_t_reg& param, double& temp, year_month& date);
//void dfsigpen(par_t_reg& dfparam, par_t_reg& param, year_month& date);

d3_array make_d3_array(int sl,int sh, int nrl, int nrh, const ivector& ncl,
           const ivector& nch);

void clean(dvector& g, int nvar, par_t_nn& pdfpar, coff_t& dfcoff,
       dmatrix& dfrelease,d3_array& dfz, double& plike);

void get_effort_array(par_t& param, indexed_regional_fishery_record& irfr,
       d3_array& effort, year_month& date,
       ivector& effort_occured);

void dftotal_mort_comp(par_t& param, par_t& dfparam, d3_array& f_mort,
       d3_array& dff_mort, dmatrix& dfz);

//void df_fish_mort_comp(par_t& dfparam, d3_array& dff_mort, d3_array& effort);

void reverse(double& like, dvector& g, int nvar);

extern int _global_report_flag;     //Set in main(); 0 while in fmin loop else 1
extern setsaveinterflag interflag;
extern intersavetype * isp;
extern ofstream clogf;

void par_t_nn::fgcomp(double& plike, dvector& x, dvector& g, int _nvar,
       recaptype_vector& recaps, int nrec,
       coff_t& coff, coff_t& dfcoff,
       indexed_regional_fishery_record& irfr)
{
  int   i, j, cohort;

  //clogf.setf(ios::fixed);

  clock_t time1 = clock();

  par_t_nn dfparam(*this);

  imatrix jlbm(1,nfleet,1,m);
  imatrix jubm(1,nfleet,1,m);
  ivector ilbv(1,nfleet);
  ivector iubv(1,nfleet);
  for (int s = 1; s <= nfleet; s++)
  {
    ilbv(s) = 1;
    iubv(s) = m;
    //jlbm(s) = jlb;
    //jubm(s) = jub;
    for (int i = 1; i <= m; i++)
    {
      jlbm(s,i) = jlb(i);
      jubm(s,i) = jub(i);
    } //end_for
  } //end_for
  d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
  d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
  d3_array dffish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
  dffish_mort.initialize();


  dmatrix tot_mort(1,m, jlb, jub);     //tot_mort.initialize();
  dmatrix dftot_mort(1,m, jlb, jub); dftot_mort.initialize();
  //Tags
  dmatrix release(1,m, jlb, jub); release.initialize();
  dmatrix dfrelease(1, m, jlb, jub); dfrelease.initialize();


  ivector effort_occured(1, nfleet); effort_occured.initialize();

#if !defined(__SUN__) && !defined(__GNUC__)
  if ( ( isp->savefile = open(isp->savefilename, O_RDWR|O_CREAT|O_BINARY,S_IREAD|S_IWRITE) ) < 0)
#else
#if defined ( __SUN__) ||  defined ( __GNUC__)
  if ( ( isp->savefile = open(isp->savefilename, O_RDWR | O_CREAT | O_TRUNC , 0660) ) < 0)
  //if ( ( isp->savefile = open(isp->savefilename, O_RDWR | O_CREAT | O_TRUNC , 0777) ) < 0)
#else
  if ( ( isp->savefile = creat(isp->savefilename, O_RDWR) ) < 0)
#endif
#endif
  {
    cerr <<"Cannot open " << isp->savefilename <<" for output.\n";
    exit(1);
  }

  plike = 0.0;
  fpen = 0.0;
  total_uv_penalty = 0;
  total_sigma_penalty = 0;
  total_negative_tag_penalty = 0;
  total_total_tag_penalty = 0 ;
  total_bounds_penalty = 0;

  //set usergrid, mort, q
  reset(x);
  plike -= fpen;
  total_bounds_penalty += fpen;

  int ncohort = nrelease;


  int pool_tags = m_ipar[13];
  TRACE(pool_tags)
  int omit_release_month = 0;
  if (m_ipar[14])
  {
    cerr << "Omit release month option (flag 14) not currently supported." << endl;
    exit(1);
  } //end_if
  int last_rn = 0;
  int c1 = 1;
  int c2 = ncohort;

  if (pool_tags == 1)
  {
    cohort = 0;
    c2 = 1;
  } //end_if
  else if (pool_tags == 2)
  {
    last_rn = 0;
    c1 = 1;
    c2 = ncohort;
  } //end_if

  //Use a macro for cleaner code
  ivector jlb1(0,m+1);
  ivector jub1(0,m+1);
  for (i = 1; i <= m; i++)
  {
    jlb1(i) = jlb(i) - 1;
    jub1(i) = jub(i) + 1;
  } //end_for
  jlb1(0) = 0;
  jlb1(m+1) = 0;
  jub1(0) = n+1;
  jub1(m+1) = n+1;
  
 // u  v and sigma  initializtion was commented out earlier.  msa  2/12/2002 8:10PM
  dmatrix u(0, m+1, jlb1, jub1);          u.initialize();
  dmatrix v(0, m+1, jlb1, jub1);          v.initialize();
  dmatrix sigma(0, m+1, jlb1, jub1);   sigma.initialize();

  dmatrix df_u(0, m+1, jlb1, jub1); df_u.initialize();
  dmatrix df_v(0, m+1, jlb1, jub1); df_v.initialize();
  dmatrix df_sigma(0, m+1, jlb1, jub1); df_sigma.initialize();


  year_month date;
  year_month local_start_date = start_date;
  year_month save_start_date = start_date;
  year_month final_date = get_tr_date(nrelease) + nmonth;

  double dfplike = 1.0;
  double dfneg_tag_pen = 0.0;
  double dftot_tag_pen = 0.0;
  dfcoff.clean();
  g.initialize();

  for (int cc = c1; cc <= c2; cc++)
  {
    interflag.isp_reset();
    if (pool_tags == 2)
    {
      cohort = get_tr_cohort(cc);
      //clogf << "\nReleasing tags for cohort " << cohort << endl;
      local_start_date = get_tr_date(cc);
      final_date = local_start_date + nmonth;
      start_date = local_start_date;
      release.initialize();
      while( (last_rn < ncohort) &&
             (cohort == get_tr_cohort(last_rn+1)) )
      {
        last_rn++;
        i = get_tr_i(last_rn);
        j = get_tr_j(last_rn);
        release(i,j) += get_tr_tags(last_rn);
        //clogf << local_start_date <<", released "
        //      << get_tr_tags(last_rn) << " at " << i << "," << j << endl;
      } //end_while
      cc = last_rn;
    } //end_if

    int total_months = final_date - start_date + 1;
    //ivector coff_comp_called(0,total_months);
    //coff_comp_called.initialize();
    int kmonth = 0;
    clogf << "---------- starting forward loop ----------" << endl;
    for (date = local_start_date; date <= final_date; date++)
    {
      kmonth ++;
      TTRACE(kmonth,date)
      if (pool_tags == 1)
      {
        while((last_rn < ncohort)
            && (date == get_tr_date(last_rn+1)))
        {
          last_rn++;
          i = get_tr_i(last_rn);
          j = get_tr_j(last_rn);
          release(i,j) += get_tr_tags(last_rn);
          //clogf << date <<", released " << get_tr_tags(last_rn) << " at "
          //      << i << "," << j << endl;
        } //end_while
      } //end_if


      uvs_comp(u, v, sigma, date);
  
      plike += 1e-8*norm2(u);
      plike += 1e-8*norm2(v);
      plike += 1e-8*norm2(sigma);

    }                                  //End of forward month loop

    interflag.iclose(isp);

    //clogf << "\n---------- starting reverse loop ----------" << endl;

    for (date = final_date; date >= local_start_date; date--)
    {
      //plike += 1e-9*norm2(sigma);
      uvs_comp(u, v, sigma, date); // recompute

      //plike += 1e-8*norm2(sigma);
      df_sigma += 1e-8*dfplike*2.0*sigma;

      //plike += 1e-8*norm2(v);
      df_v += 1e-8*dfplike*2.0*v;

      //plike += 1e-8*norm2(u);
      df_u += 1e-8*dfplike*2.0*u;

      dfparam.dfuvs_comp(*this, df_u, df_v, df_sigma, date);

      kmonth --;
    } //End reverse month loop

    if (pool_tags == 2)
    {
      //cohort = get_tr_cohort(cc);
      //local_start_date = get_tr_date(cc);
      //final_date = local_start_date + nmonth;

      //release.initialize();
      dfrelease.initialize();

      //while((last_rn < ncohort)
      //            && (cohort == get_tr_cohort(last_rn+1)))
      //{
      //  last_rn++;
      //  i = get_tr_i(last_rn);
      //  j = get_tr_j(last_rn);
      //  release(i,j) += get_tr_tags(last_rn);
      //  clogf << local_start_date <<", released "
      //        << get_tr_tags(last_rn) << " at " << i << "," << j << endl;
      //}
    } //end_if

  } //for (int cc = c1; cc <= c2; cc++)
  //End of cohort loop

  start_date = save_start_date;
  dfparam.dfreset(g, x);
  reverse(plike, g, _nvar);

  close(isp->savefile);

  if (_global_report_flag)
    resid_comp(release, release, release, 1);

  time_t time_sec;
  time(&time_sec);
  clock_t time2 = clock();
  double elapsed_time = (double)(time2-time1)/CLOCKS_PER_SEC;
  clogf << "Objective function value = " << setprecision(8) << plike
        << "; elapsed time = " << setprecision(2) << elapsed_time
        << " seconds." << endl;
}//End of fgcomp

