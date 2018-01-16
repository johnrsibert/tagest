#include "precomp.h"
#pragma hdrstop
#include <prnstrem.h>
#include <iomanip>
//#include "trace.h"

#ifdef __TURBOC__
  #undef __ZTC__
  extern unsigned _stklen = 0x4000;
#endif

#ifdef __ZTC__
  unsigned _stack = 64000;
#endif
//void make_ext(const char c, const int n, char * ext);


adstring get_parent_directory(adstring& path);
adstring get_file_name_extension(adstring& path);
//int nvarcalc(par_t& p);
//void xinit(dvector& x, par_t& p, adstring_array& x_names);
//void xinit(dvector& x, par_t& p, boundstype& bds, adstring_array& name);

ofstream clogf("fit_tab.log");

int main()
{
  cifstream ftl("fits.lst");
  if ( !ftl)
  {
    cerr << "Cannot open fits.lst" << endl;
    exit(1);
  }

  int max_nvar = 0;
  dvector x;
  dmatrix xx;
  adstring_array x_names;
  adstring_array x_names_max;

  int nvar = 0;
  int nextra_vars = 20;

  int nfile = 0;
  ftl >> nfile;

  adstring_array ext(1,nfile);

  for (int ff = 1; ff <= nfile; ff++)
  {
    char pathname[80];
    ftl >> pathname;
    if (ftl.good())
    {
      cout << "Read '" << pathname << "' from fits.lst" << endl;

      par_t_reg param(pathname);
      param.m_ipar[26]=0; // surpress region active stuff

      if (!xx)
      {
        nvar = param.nvarcalc();
        xx.allocate(1,nvar+nextra_vars,1,nfile);
        xx.initialize();
        x_names.allocate(1,nvar);
        x_names_max.allocate(1,nvar);
        x.allocate(1,nvar);
      }
    
      int nvar_test = param.nvarcalc();
      if (nvar != nvar_test)
      {
        cout << "nvar incorrect for file " << pathname << endl;
        cout << "current nvar should be " << nvar 
             << "; new value was " << nvar_test << endl;
        exit(1);
      } 

      param.xinit(x, x_names);
      //int nvar_test = param.nvarcalc();
      if (max_nvar <= 0)
      {
        max_nvar = nvar;
        for (int n = 1; n <= nvar; n++)
        {
          x_names_max(n) = x_names(n);
        }
      }
      else if (nvar > max_nvar)
      {
        cerr << "nvar exceeds max_nvar for file " << pathname << endl;
        exit(1);
      }

      adstring path(pathname);

      adstring sub_dir = get_parent_directory(path);

      ext(ff) = get_file_name_extension(path);

      int i = 1;

      for (int ii = 1; ii <= nvar; ii++)
      {
        while ( (i <= max_nvar) && (x_names(ii) != x_names_max(i)) )
        {
          i++;
        }
        xx(i,ff) = x(ii);
      }

      i++;
      xx(i,ff) = param.get_nvar();
      i++;
      xx(i,ff) = param.get_objective_function_value();
      i++;
      xx(i,ff) = param.m_ipar[31];
      i++;
      xx(i,ff) = param.m_ipar[32];
      i++;
      xx(i,ff) = param.m_ipar[42];
      i++;
      xx(i,ff) = param.m_ipar[6];
      i++;
      xx(i,ff) = param.get_nmonth();
      i++;
      xx(i,ff) = param.get_nfleet();
      i++;
      xx(i,ff) = param.get_nrelease();
      i++;
      xx(i,ff) = param.get_minvel();
      i++;
      xx(i,ff) = param.get_maxvel();
      i++;
      xx(i,ff) = param.get_minsig();
      i++;
      xx(i,ff) = param.get_maxsig();

      // these variables not currently read by par file input function
      i++;
      xx(i,ff) = param.get_total_uv_penalty();
      i++;
      xx(i,ff) = param.get_total_sigma_penalty();
      i++;
      xx(i,ff) = param.get_total_negative_tag_penalty();
      i++;
      xx(i,ff) = param.get_total_total_tag_penalty();
      i++;
      xx(i,ff) = param.get_total_bounds_penalty();

      i++;
      xx(i,ff) = param.m_ipar[5];
      i++;
      xx(i,ff) = param.get_maximum_gradient();

    }
  }
  ftl.close();

  prnstream prn("fit_tab.prn");


  prn << "  ";
  for (int ff = 1; ff <= nfile; ff++)
  {
#if defined(__BCPLUSPLUS__) || defined(__GNUC__)
    prn << (char*)ext(ff);
#else
    prn << ext(ff);
#endif
  }
  prn << PENDL;

  //for (int i=1; i <= max_nvar+13; i++) //nextra_vars; i++)
  for (int i=1; i <= max_nvar+nextra_vars; i++)
  {
    if (i <= nvar)
    #if defined(__BCPLUSPLUS__) || defined(__GNUC__)
      prn << (char*)x_names_max(i);
    #else
      prn << x_names_max(i);
    #endif
    else if (i == nvar+1)
      prn << "nvar";
    else if (i == nvar+2)
      prn << "L";
    else if (i == nvar+3)
      prn << "u curv";
    else if (i == nvar+4)
      prn << "v curv";
    else if (i == nvar+5)
      prn << "sigma curv";
    else if (i == nvar+6)
      prn << "time steps";
    else if (i == nvar+7)
      prn << "nmonth";
    else if (i == nvar+8)
      prn << "fleets";
    else if (i == nvar+9)
      prn << "cohorts";
    else if (i == nvar+10)
      prn << "u,v min";
    else if (i == nvar+11)
      prn << "u,v max";
    else if (i == nvar+12)
      prn << "D min";
    else if (i == nvar+13)
      prn << "D max";
    else if (i == nvar+14)
      prn << "P_uv";
    else if (i == nvar+15)
      prn << "P_D";
    else if (i == nvar+16)
      prn << "P_nt";
    else if (i == nvar+17)
      prn << "P_tt";
    else if (i == nvar+18)
      prn << "P_bnd";
    else if (i == nvar+19)
      prn << "fn_evals";
    else if (i == nvar+20)
      prn << "max(|g|)";
	
    for (int ff = 1; ff <= nfile; ff++)
    {
      prn << xx(i,ff);
    }
    prn << PENDL;
    if (i == nvar)
      prn << PENDL;
  }

  prn << PENDL;



  prn.close();

  return 0;
} // end of main


adstring get_parent_directory(adstring& path)
{

  int dir_level = 0;
  int pnl = path.size();
  int d1 = pnl+1;
  int d2 = pnl;
  while (dir_level < 2)
  {
    d1 --;
  #ifdef unix
    if (path(d1) == '/')
  #else
    if (path(d1) == '\\')
  #endif
    {
      dir_level ++;
      if (dir_level < 2)
      d2 = d1;
    }
  } // while (dir_level < 2)

  adstring parent_directory = path(d1+1, d2-1);
  return(parent_directory);
}

adstring get_file_name_extension(adstring& path)
{
  int pnl = path.size();
  int dot_pos = pnl;
  while ( (dot_pos > 1) && (path(dot_pos) != '.') )
    dot_pos --;
  adstring ext = path(dot_pos+1,pnl);
  return(ext);
}
