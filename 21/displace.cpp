#include "precomp.h"
#include "par_t.h"
#include <prnstrem.h>
#ifdef unix
#include <strstream.h>
#else
#include <strstrea.h>
#endif

#include "trace.h"

extern ofstream clogf;
// observed displacements from recapture file
void displacement_frequency(recaptype_vector& recaps, int nrecaps, par_t& param,
                            dmatrix& displacements, 
                            dmatrix& xdisp, dmatrix& ydisp, const imatrix& bins)
{
  displacements.initialize();
  xdisp.initialize();
  ydisp.initialize();
  int kmax = displacements(0).indexmax();
  int nrelease = param.get_nrelease();
  int max_month = displacements.rowmax();
  int month = 0;
  for (int kc = 1; kc <= nrecaps; kc++)
  {
     int cohort = recaps[kc].cohort;
     int i = recaps[kc].i;
     int j = recaps[kc].j;
     for (int cc = 1; cc <= nrelease; cc++)
     {
       if (cohort == param.get_tr_cohort(cc))
       {
         break;
       }
     }
     int tal = recaps[kc].date - param.get_tr_date(cc);
     if (tal > max_month)
     {
       tal = max_month;
     }
     int irel = param.get_tr_i(cc);
     int jrel = param.get_tr_j(cc);
     int ii = i-irel;
     int jj = j-jrel;
     xdisp(tal,ii) += recaps[kc].returns;
     ydisp(tal,jj) += recaps[kc].returns;

     ii = abs(ii);
     jj = abs(jj);
     int kk = bins(ii,jj);
     if (kk > kmax)
        kk = kmax;
     displacements(tal,kk) += recaps[kc].returns;
  }
}

// diplacements of non-recaptured tags at liberty
void dispacement_frequency(const int cohort, const dmatrix& tags, 
                           dvector& displacements, 
                           dvector& xdisp, dvector& ydisp, 
                           const imatrix& bins, par_t& param)
{
  int irel = param.get_tr_i(cohort);
  int jrel = param.get_tr_j(cohort);

  int i1 = tags.rowmin();
  int i2 = tags.rowmax();
  int kmax = displacements.indexmax();
  for (int i = i1; i <= i2; i++)
  {
    int j1 = tags(i).indexmin();
    int j2 = tags(i).indexmax();
    for (int j = j1; j <= j2; j++)
    {
      int ii = i-irel;
      xdisp(ii) += tags(i,j);
      int jj = j-jrel;
      ydisp(jj) += tags(i,j);

      ii = abs(ii);
      jj = abs(jj);
      int kk = bins(ii,jj);
      if (kk > kmax)
        kk = kmax;
      displacements(kk) += tags(i,j);
    }
  }
}

// displacement distance bin index for pairwise displacments in model area 
imatrix displacement_bins(par_t& param, const double bin_width)
{
  int m = param.get_m();
  int n = param.get_n();
  imatrix bins(0,m,0,n);
  bins.initialize();
  const double dx = param.get_deltax_prn();
  const double dy = param.get_deltay(); 
  for (int i = 0; i <= m; i++)
  {
    double x = i*dx;
    for (int j = 0; j <= n; j++)
    {
      double y = j*dy;
      double d = sqrt(x*x+y*y)/bin_width+0.1;
      TTRACE(sqrt(x*x+y*y),int(d))
      bins(i,j) = int(d);
    }
  }
  return(bins);
}

void write_displacement_files(const dmatrix& displacement, 
                         dmatrix& xdisp, dmatrix& ydisp, const double bin_width,
                         const int fit_number, const char op)
{
  char fit_name[80];
  ostrstream ss(fit_name,80);
  ss << "fit" << setw(2) << setfill('0') << hex << fit_number << '_' << op << ends;
  
  adstring displacement_name = fit_name+adstring("displ.prn");
  TRACE(displacement_name)
  prnstream dfp(displacement_name);
  dfp << "displacement frequencies" << PENDL;
  dfp << "months at"  << "net displacement (Nmi) ->" << PENDL;
  dfp << "liberty";
  int d1 = displacement(0).indexmin();
  int d2 = displacement(0).indexmax();
  int d = 0;
  for (d = d1; d <= d2; d++)
  {
    dfp << (d*bin_width);
  }
  dfp << PENDL;
  int max_month = displacement.rowmax();
  int month = 0;
  for (month = 0; month <= max_month; month++)
  {
    dfp << month;
    for (d = d1; d <= d2; d++)
    {
      dfp << displacement(month,d); 
    }
    dfp << PENDL;
  }
  dfp.close();

  
  displacement_name = fit_name+adstring("Xdispl.prn");
  TRACE(displacement_name)
  prnstream dfx(displacement_name);
  dfx << "E-W displacement frequencies" << PENDL;
  dfx << "months at"  << "net displacement (Nmi) ->" << PENDL;
  dfx << "liberty";
  d1 = xdisp(0).indexmin();
  d2 = xdisp(0).indexmax();
  for (d = d1; d <= d2; d++)
  {
    dfx << (d*60); //delta_x
  }
  dfx << PENDL;
  max_month = displacement.rowmax();
  for (month = 0; month <= max_month; month++)
  {
    dfx << month;
    for (d = d1; d <= d2; d++)
    {
      dfx << xdisp(month,d); 
    }
    dfx << PENDL;
  }
  dfx.close();

  displacement_name = fit_name+adstring("Ydispl.prn");
  TRACE(displacement_name)
  prnstream dfy(displacement_name);
  dfy << "N-S displacement frequencies" << PENDL;
  dfy << "months at"  << "net displacement (Nmi) ->" << PENDL;
  dfy << "liberty";
  d1 = ydisp(0).indexmin();
  d2 = ydisp(0).indexmax();
  for (d = d1; d <= d2; d++)
  {
    dfy << (d*60); //delta_y
  }
  dfy << PENDL;
  max_month = displacement.rowmax();
  for (month = 0; month <= max_month; month++)
  {
    dfy << month;
    for (d = d1; d <= d2; d++)
    {
      dfy << ydisp(month,d); 
    }
    dfy << PENDL;
  }
  dfy.close();
}
