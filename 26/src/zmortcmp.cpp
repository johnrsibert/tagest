//$Id: zmortcmp.cpp 2768 2011-02-18 02:49:17Z jsibert $
#include "par_t.h"
#include "trace.h"

 
void par_t::total_mort_comp(dvar3_array& f_mort1, dvar3_array& f_mort0, dvar_matrix& z)
{
   if (m_ipar(53) == 0)
     total_mort_comp(f_mort1,z);
   else
   {
     dvar3_array f_mort = f_mort1+f_mort0;
     total_mort_comp(f_mort, z);
   }
}
 
void par_t::total_mort_comp(dvar3_array& f_mort, dvar_matrix& z)
{
  if (m_ipar[19] > 0)
  {
    int special_region = m_ipar[19];
    for (int i=1; i<=m; i++)
    {
      int j1 = jlb(i);
      int jn = jub(i);
      for (int j=j1; j<=jn; j++)
      {
        int k = gridmap(i,j);
        if (k==special_region)
         z(i,j) = special_mort;
        else
         z(i,j) = mort;
      }
    }
  }
  else
  {
    z = mort;
  }  
  dvariable s = 1.0;
  if (m_ipar[20] == 0)
    s = 1.0;
  else if (m_ipar[20] == 1)
    s = 1.0 - mort;
  else
  {
     clogf << "Illegal value of m_ipar[20] passed to total_mort_comp" << endl;
     exit(1);
  }

  for (int f=1; f <= nfleet; f++)
  {
    if (m_ipar[20] == 0)
    {
      z += f_mort(f);
    }
    else if (m_ipar[20] == 1)
    {
      z += f_mort(f)*s;  // i.e. Z = M + F - F*M = M + F*(1 - M)
    }
  }
}

 
void par_t::total_mort_comp(dvar3_array &f_mort, dvar_matrix &z, dvariable &mort, dvariable &special_mort)
{
  if (m_ipar[19] > 0) {
    const int special_region = m_ipar[19];
    for (int i = 1; i <= m; i++) {
      const int jmin = jlb(i);
      const int jmax = jub(i);
      for (int j = jmin; j <= jmax; j++) {
        int k = gridmap(i,j);
        if (k==special_region)
          z(i,j) = special_mort;
        else
          z(i,j) = mort;
      }
    }
  } else {
    z = mort;
  }
  dvariable s = 1.0;
  if (m_ipar[20] == 0)
    s = 1.0;
  else if (m_ipar[20] == 1)
    s = 1.0 - mort;
  else {
    clogf << "Illegal value of m_ipar[20] passed to total_mort_comp" << endl;
    exit(1);
  }
  for (int f=1; f <= nfleet; f++) {
    if (m_ipar[20] == 0) {
      z += f_mort(f);
    } else if (m_ipar[20] == 1) {
      z += f_mort(f)*s;  // i.e. Z = M + F - F*M = M + F*(1 - M)
    }
  }
}

/*
#ifndef SIMULATE
void dftotal_mort_comp(par_t& dfparam, d3_array& f_mort, d3_array& dff_mort, dmatrix& dfz)
{
  double s = 1.0;
  if (param.m_ipar[20] == 1)
  {
    s = 1.0-param.get_mort();
  }
  double dfs = 0.0;

  for (int f=1; f <= dfparam.get_nfleet(); f++)
  {
    if (param.m_ipar[20] == 0)
    {
      //z += f_mort(f);
      dff_mort(f) += dfz;
    }
    else if (param.m_ipar[20] == 1)
    {
      //z += f_mort(f)*s;
      dff_mort(f) += dfz*s;
 
      dfs += sum(elem_prod(dfz,f_mort(f)));
    }

  }
  if (param.m_ipar[20] == 1)
  {
    //double s = 1.0-param.mort;
    //dfparam.mort -= dfs;
    double temp_mort = dfparam.get_mort() - dfs;
    dfparam.set_mort(temp_mort);
    dfs = 0.0;
  }

  if (param.m_ipar[19] > 0)
  {
    int special_region = param.m_ipar[19];
    double dfmort = dfparam.get_mort();
    double dfspecial_mort = dfparam.get_special_mort();
    int m = param.get_m();
    ivector& jlb = param.get_jlb();
    ivector& jub = param.get_jub();
    imatrix& gridmap = param.get_gridmap();
    for (int i=m; i>=1; i--)
    {
      int j1 = jlb(i);
      int jn = jub(i);
      for (int j=jn; j>=j1; j--)
      {
        int k = gridmap(i,j);
        if (k==special_region)
        {
           //z(i,j) = special_mort;
          dfspecial_mort += dfz(i,j);
          dfz(i,j) = 0.0;
        }
        else
        {
          //z(i,j) = mort;
          dfmort += dfz(i,j);
          dfz(i,j) = 0.0;
        }
      }
    }
    //dfparam.mort = dfmort;
    dfparam.set_mort(dfmort);
    dfparam.set_special_mort(dfspecial_mort);
  }
  else
  {
    double dfmort = dfparam.get_mort();
    //z = param.mort;
    dfmort += sum(dfz);
    dfparam.set_mort(dfmort);
    dfz.initialize();
  }

}
#endif
*/
