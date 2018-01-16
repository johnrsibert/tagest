#include "precomp.h"

double normal_dev(const double x, const double mu, const double sigma);
#define TRACE(object) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #object " = " << object << endl;
#define TTRACE(o1,o2) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #o1 " = " << o1<< ", " << #o2 " = " << o2 << endl;
extern ofstream clogf;
double randn(long int& n);
adstring recruitment_file_name(year_month& date); //, char* _directory);
int move_corner(const int x, const int edge);

void linbcg::g_comp(par_t& param, const dmatrix& recruitment)
{
  //TRACE(g_)
  double dt = 1.0/param.m_ipar[6];
  for (int i = 1; i <= m_; i++)
  {
     int j1 = recruitment(i).indexmin();
     int j2 = recruitment(i).indexmax();
     //cout << j0 << " & " << jN << endl;
     for (int j = j1; j <= j2; j++)
     {
       g_((j - 1) * m_ + i) = recruitment(i, j);
     } // for j
  }//for i
  //TRACE(g_)
}

void par_t_reg::population_initialize(dmatrix& pop,
                                      const double init_pop)
{
    int ncell = 0;
    for (int i = 1; i <= m; i++)
    {
      int jlb = pop(i).indexmin();
      int jub = pop(i).indexmax();
      for (int j = jlb; j <= jub; j++)
      {
	      if (gridmap(i,j) > 0)
        {
       	 pop(i,j) = init_pop;
          ncell++;
        }
	      else
	        pop(i,j) = 0.0;
      }
    }
}


#include <values.h>
double min(const dmatrix& t)
{
  double x = MAXDOUBLE;
  int i1 = t.rowmin();
  int i2 = t.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    double tmp = min(t(i));
    if (tmp < x)
      x = tmp;
  }
  return(x);
}


#include <values.h>
void par_t_reg::recruit_comp(dmatrix& r, const dmatrix& pop,
                const year_month& date, double& spawning_biomass)
{
  spawning_biomass = 0.0;
  double popmin = MAXDOUBLE;
  double eps = 0.00001;
  int mode_flag_m_ipar = 131;
  int mode_flag = m_ipar[mode_flag_m_ipar];
  if (mode_flag == 0)
  {
    double rcell = mort/1714.0;
    for (int i = 1; i <= m; i++)
    {
      int jlb = r(i).indexmin();
      int jub = r(i).indexmax();
      for (int j = jlb; j <= jub; j++)
      {
	       if (gridmap(i,j) > 0)
         {
           if (pop(i,j) < popmin)
             popmin = pop(i,j);
           if (pop(i,j) > eps)
           {
       	     r(i,j) = rcell;
             spawning_biomass += pop(i,j);
           }
         }
	       else
	         r(i,j) = 0.0;
      }
    }
    TRACE(popmin)
    TRACE(sum(r))
  }

  else if (mode_flag == 3)
  {
  //i = move_corner(int(t.Long.value()),sw_long);
  //j = move_corner(int(t.Lat.value()),sw_lat);
    int sw_long = (int)sw_coord.long_value();
    int sw_lat = (int)sw_coord.lat_value();

    int i1 = move_corner(m_ipar(126),sw_long);
    int j1 = move_corner(m_ipar(127),sw_lat);
    int i2 = move_corner(m_ipar(128),sw_long);
    int j2 = move_corner(m_ipar(129),sw_lat);

    int ilb = max(1,i1);
    int iub = min(m,i2);

    double ncell = 0.0;
    double rcell = (double)m_ipar(132)*mort;
    for (int i = i1; i <= i2; i++)
    {
       int jlb = max(r(i).indexmin(),j1);
       int jub = min(r(i).indexmax(),j2);
       for (int j = jlb; j <= jub; j++)
       {
	      if (gridmap(i,j) > 0)
         {
           ncell ++;
           if (pop(i,j) < popmin)
             popmin = pop(i,j);
           if (pop(i,j) > eps)
           {
       	     r(i,j) = rcell;
             spawning_biomass += pop(i,j);
           }

         }
	       else
	         r(i,j) = 0.0;
      }
    }
    double rncell = 1.0/ncell;
    r = r*rncell;
  }

  else if (mode_flag ==4)
  {
    dmatrix sst(1,m,jlb,jub);
    int month = date.get_month_value();
    const int oblen = 20;
    char ob2[oblen];
    ostrstream sob2(ob2,oblen);
  #ifdef unix
    sob2 << "../nodc00" << setw(2) << setfill('0') << month  << ".sst" << '\0';
   #else
    sob2 << "..\\nodc00" << setw(2) << setfill('0') << month  << ".sst" << '\0';
  #endif
    ifstream sstf(ob2);
    if (!sstf)
    {
      cerr << "Error opening sst input file"
           <<"'" << ob2 << "'" << endl;
      exit(1);
    }
    sstf >> sst;
    clogf << "read sst from " << ob2 << endl;

    double tmin = m_ipar(133);
    double tmax = m_ipar(134);

    double ncell = 0.0;
    double rcell = (double)m_ipar(132)*mort;
    for (int i = 1; i <= m; i++)
    {
      int j1 = r(i).indexmin();
      int j2 = r(i).indexmax();
      for (int j = j1; j <= j2; j++)
      {
	      if ( (gridmap(i,j) > 0) &&
              (sst(i,j) >= tmin) && (sst(i,j) < tmax) )
         {
             ncell ++;
             if (pop(i,j) < popmin)
               popmin = pop(i,j);
             if (pop(i,j) > eps)
             {
       	      r(i,j) = rcell;
               spawning_biomass += pop(i,j);
             }

         }
	       else
	         r(i,j) = 0.0;
      }
    }
    TRACE(ncell)
    double rncell = 1.0/ncell;
    //TRACE(sum(r))
    r = r*rncell;
    //TRACE(sum(r))

  }

  else if (mode_flag == 5)
  {
    dmatrix sst(1,m,jlb,jub);
    int month = date.get_month_value();
    const int oblen = 20;
    char ob2[oblen];
    ostrstream sob2(ob2,oblen);
  #ifdef unix
    sob2 << "../nodc00" << setw(2) << setfill('0') << month  << ".sst" << '\0';
   #else
    sob2 << "..\\nodc00" << setw(2) << setfill('0') << month  << ".sst" << '\0';
  #endif
    ifstream sstf(ob2);
    if (!sstf)
    {
      cerr << "Error opening sst input file"
           <<"'" << ob2 << "'" << endl;
      exit(1);
    }
    sstf >> sst;
    clogf << "read sst from " << ob2 << endl;

    double tmin = m_ipar(133);
    double tmax = m_ipar(134);

    double ncell = 0.0;
    double rcell = (double)m_ipar(132)*mort;

    TRACE(rcell)
    double d_bh = 0.5/rcell; // maximum at 1/n equilibrium
    double c_bh = exp(d_bh*rcell); // replacement at equilibrium
    TTRACE(d_bh,c_bh)
    
    for (int i = 1; i <= m; i++)
    {
      int j1 = r(i).indexmin();
      int j2 = r(i).indexmax();
      for (int j = j1; j <= j2; j++)
      {
	      if ( (gridmap(i,j) > 0) &&
              (sst(i,j) >= tmin) && (sst(i,j) < tmax) )
         {
             ncell ++;
             if (pop(i,j) < popmin)
               popmin = pop(i,j);
             if (pop(i,j) > eps)
             {
       	      r(i,j) = c_bh*exp(-d_bh*pop(i,j));
               spawning_biomass += pop(i,j);
             }

         }
	       else
	         r(i,j) = 0.0;
      }
    }
    TRACE(ncell)
    double rncell = 1.0/ncell;
    //TRACE(sum(r))
    r = r*rncell;
    //TRACE(sum(r))

  }
  /*
  else if (mode_flag == 1)
  {
    int n = _get_n();
    dvector average_recruitment(1,n);
    double rmax = 0.0;
    for (int j = 1; j <= n; j++)
    {
      double latitude= sw_lat + (deltay*double(j))/60.0;
      average_recruitment(j) = normal_dev(latitude, 0.0, 5.0);
      if (average_recruitment(j) > rmax)
	   rmax = average_recruitment(j);
    }
    average_recruitment /= rmax;
    double rmean = (double)m_ipar[mode_flag_m_ipar+1];
    double rsig = (double)m_ipar[mode_flag_m_ipar+2]/100.0;
    long seed = m_ipar[40];

    for (int i = 1; i <= m; i++)
    {
      int jlb = g(i).indexmin();
      int jub = g(i).indexmax();
      for (int j = jlb; j <= jub; j++)
      {
	if (gridmap(i,j) > 0)
	{
	  g(i,j) = rmean*average_recruitment(j)*exp(rsig*randn(seed));
	}
	else
	  g(i,j) = 0.0;
      }
    }

    adstring rfile = recruitment_file_name(date);

    ofstream rf(rfile);
    rf << g << endl;

    m_ipar[40] = seed;
  }

  else if (mode_flag == 2)
  {
    adstring rfile = recruitment_file_name(date);

    ifstream rf(rfile);
    rf >> g;
    if (!rf)
    {
      cerr << "Error reading file "<< rfile << " in gcomp" << endl;
      clogf << "Error reading file "<< rfile << " in gcomp" << endl;
      exit(1);
    }
  }
  */
  else
  {
    cerr << "Unknown mode flag ** (" << mode_flag << ") passed to gcomp." << endl;
    clogf << "Unknown mode flag ** (" << mode_flag << ") passed to gcomp." << endl;
    exit(1);
  }
}

double normal_dev(const double x, const double mu, const double sigma)
{
  double v = 0.0;
  double t = (x-mu)/sigma;
  if (t < 80.0)
  {
    v = exp(-t*t)/sqrt(6.2831853 * sigma);
  }
  return(v);
}

adstring recruitment_file_name(year_month& date) //, char* _directory)
{
  /*
  adstring directory;
  if (_directory)
    directory = adstring(_directory);
  else
  {
    directory = "..\\";
    #if defined __GNUC__ || __SUN__
      directory = "../";
    #endif
  }
  */
  int month = date.get_month().get_value();
  char month_str[3];
  sprintf(month_str,"%02d",month);
  char year_str[3];
  //sprintf(year_str,"%02d",date.get_year());
  sprintf(year_str,"%02d",0);
  adstring file_name = adstring(year_str)+month_str+".rec";
  //TRACE(file_name)

#if defined __SUN__ || __GNUC__
  file_name.to_lower();
#endif

  return (file_name);
}
