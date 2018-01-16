#include "precomp.h"
#include "par_t.h"
#include <prnstrem.h>

extern ofstream clogf;
// observed displacements from recapture file
void displacement_frequency(recaptype_vector& recaps, int nrecaps, par_t& param,
                            dmatrix& displacements, const imatrix& bins)
{
  displacements.initialize();
  int kmax = displacements(0).indexmax();
  int nrelease = param.get_nrelease();
  int max_month = displacements.rowmax();
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
     int ii = abs(irel-i);
     int jj = abs(jrel-j);
     int kk = bins(ii,jj);
     if (kk > kmax)
        kk = kmax;
     displacements(tal,kk) += recaps[kc].returns;
  }
}

// diplacements of non-recaptured tags at liberty
void dispacement_frequency(const int cohort, const dmatrix& tags, 
                           dvector& displacements, const imatrix& bins,
                           par_t& param)
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
      int ii = abs(irel-i);
      int jj = abs(jrel-j);
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
      double d = sqrt(x*x+y*y)/bin_width;
      bins(i,j) = int(d);
    }
  }
  return(bins);
}

void write_displacement_file(const dmatrix& displacement, const adstring& label)
{
  adstring displacement_name = label + adstring(".prn");
  prnstream dfp(displacement_name);
  dfp << "displacement frequencies" << PENDL;
  dfp << "months at"  << "net displacement (Nmi) ->" << PENDL;
  dfp << "liberty";
  int d1 = displacement(0).indexmin();
  int d2 = displacement(0).indexmax();
  for (int d = d1; d <= d2; d++)
  {
    dfp << (d*60);
  }
  dfp << PENDL;
  int max_month = displacement.rowmax();
  for (int month = 0; month <= max_month; month++)
  {
    dfp << month;
    for (int d = d1; d <= d2; d++)
    {
      dfp << displacement(month,d); 
    }
    dfp << PENDL;
  }
  dfp.close();
}
