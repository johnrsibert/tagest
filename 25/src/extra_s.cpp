//$Id: extra_s.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "precomp.h"
#include <prnstrem.h>

extern ofstream clogf;
void se_write(prnstream& ofs, par_t& p, par_t& p1, par_t& p2, 
              const adstring_array& name);


int file_exists(const char* name);
void make_ext(const char c, const int n, char * ext);
int last_ext_number(const char* root, const char letter);
int mqrr_check(const adstring& name);


void sd_scale(dvector& d, dvector& x, par_t& param);

void obrien_derivs(dmatrix& S, dvector& x, dvector& scale,
                   dvector& v, int nvar_model, int nvar_extra,
                   par_t_reg& param)
{
  /*
   correct_sigma(s,k) = param.usergrid[s][k].sigma
                         + 15.0*(dx*fabs(param.usergrid[s][k].u)+
                                 dy*fabs(param.usergrid[s][k].v));
  */
  const int nseason = param.get_nseason();
  const int ngrid = param.get_ngrid();
  const int nfleet = param.get_nfleet();
  const double dx = param.get_deltax_eq();
  const double dy = param.get_deltay();

  imatrix& stratum_active = param.get_stratum_active();
  dmatrix g(1,nvar_extra,1,nvar_model);
  g.initialize();

  int s, k;
  int ii = 0;
  int jj = 0;

  // global D
  if(param.m_ipar[21] == 1)
  {
    jj ++;
    ii ++;
    g(jj,ii) = 1.0;
  }

 // season x region D
  if(param.m_ipar[21] == 2)
  {
    for (k=1; k <= ngrid; k++)
    {
      for (s=1; s <= nseason; s++)
      {
     	  if (stratum_active(s,k))
	      {
           jj++;
	        ii++;
           g(jj,ii) = 1.0;
      	}
      }
    }
  }

  // natural mortality
  if(param.m_ipar[22] == 1)
  {
    ii++;
    // special mortality
    if(param.m_ipar[19] > 0)
    {
      ii++;
    }
  }

  //uniform q
  if(param.m_ipar[25] == 1)
  {
    ii++;
  }

  //separate q for each fleet
  if(param.m_ipar[25] == 2)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
    }
  }

  //separate report_rates for each fleet
  if(param.m_ipar[28] == 1)
  {
     for (int f = 1; f <= nfleet; f++)
    {
      ii++;
    }
  }

  if (param.m_ipar[23]==1)
  {
    jj = 0;
    for (k=1; k <= ngrid; k++)
    {
      for (s=1; s<=nseason; s++)
      {
      	if (stratum_active(s,k))
	      {
          jj ++;

	        ii++;
          if (param.get_usergrid_u(s,k) < 0.0)
            g(jj,ii) = -15.0*dx;
          else
            g(jj,ii) =  15.0*dx;

	        ii++;
          if (param.get_usergrid_v(s,k) < 0.0)
            g(jj,ii) = -15.0*dy;
          else
            g(jj,ii) =  15.0*dy;

	      }
      }
    }
  }

  ii = nvar_model;
  jj = 0;
//  const int nseason = param.nseason;
//  const int ngrid = param.ngrid;
  double correct_sigma;

  for (k=1; k <= ngrid; k++)
  {
    for (s=1; s<=nseason; s++)
    {

      correct_sigma = param.get_usergrid_sigma(s,k)
                         + 15.0*(dx*fabs(param.get_usergrid_u(s,k))+
                                 dy*fabs(param.get_usergrid_v(s,k)));
      ii++;
      jj++;
      x(ii) = correct_sigma;

      double v1 = 0.0;
      for (int i = 1; i <= nvar_model; i++)
      {
        v1 += S(i,i)*g(jj,i)*g(jj,i)*scale(i)*scale(i);
      }
      double v2 = 0.0;
      for (int j = 1; j <= nvar_model; j++)
      {
        for (int i=1; i < j; i++)
        {
          v2 += S(i,j)*scale(i)*scale(j)*g(jj,i)*g(jj,j);
        }
      }
      v(ii) = v1 + 2.0*v2;
    }
  }
} // end

void sd_scale(dvector& d, dvector& x, par_t_reg& param)
//void reset(dvector& x, par_t& param, boundstype& bds)
{
  int     ii, k, s;
  imatrix& stratum_active = param.get_stratum_active();
  const int nseason = param.get_nseason();
  const int ngrid = param.get_ngrid();
  const int nfleet = param.get_nfleet();
  double fpen = param.get_fpen();

  ii = 0;
  if (param.m_ipar[24] > 0)
  {
    const int num_afm = param.m_ipar(24);
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      //afm(i) = boundp(x(ii),minafm,maxafm,fpen);
      d(ii) = boundp(x(ii),param.get_minafm(),param.get_maxafm(),fpen);
    }
  }

  if(param.m_ipar[21] == 1)
  {
    ii ++;
    for (k=1; k <= ngrid; k++)
    {
      for (s=1; s<=nseason; s++)
      {
	     if (stratum_active(s,k))
	     {
	       //param.usergrid[s][k].sigma = dfatan1(x[ii],param.get_minsig,param.get_maxsig,fpen);
	       d(ii) = boundp(x[ii],param.get_minsig(),param.get_maxsig(),fpen);
	     }
      }
    }
  }

  if(param.m_ipar[21] == 2)
  {
    for (k=1; k <= ngrid; k++)
    {
      for (s=1; s<=nseason; s++)
      {
	      if (stratum_active(s,k))
      	{
	        ii ++;
	        //param.usergrid[s][k].sigma = dfatan1(x[ii],param.get_minsig,param.get_maxsig,fpen);
	        d(ii) = boundp(x[ii],param.get_minsig(),param.get_maxsig(),fpen);
	      }
      }
    }
  }

  if(param.m_ipar[22] == 1)
  {
    ii++;
    d(ii) = boundp(x[ii],param.get_minmort(),param.get_maxmort(),fpen);
    if(param.m_ipar[19] >0)
    {
      ii++;
      d(ii) = boundp(x[ii],param.get_minmort(),param.get_maxmort(),fpen);
    }
  }

  if(param.m_ipar[25] == 1)
  {
    ii++;
    for (int f = 1; f <= nfleet; f++)
    {
      d(ii) = boundp(x[ii],param.get_minq(),param.get_maxq(),fpen);
    }
  }

  if(param.m_ipar[25] == 2)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      d(ii) = boundp(x[ii],param.get_minq(),param.get_maxq(),fpen);
    }
  }

  //clogf << "***reset" << endl;
  if(param.m_ipar[28] == 1)
  {
    //report_rates
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      d(ii) = boundp(x[ii],param.get_minr_rates(),param.get_maxr_rates(),fpen);
    }
  }

 if(param.m_ipar[23] == 1)
 {
    for (k=1; k <= ngrid; k++)
    {
      for (s=1; s <= nseason; s++)
      {
      	if (stratum_active(s,k))
	      {
	        ii++;
	        d(ii) = boundp(x[ii],param.get_minvel(),param.get_maxvel(),fpen);
	        ii++;
	        d(ii) = boundp(x[ii],param.get_minvel(),param.get_maxvel(),fpen);
      	}
      }
    }
  }

  param.set_fpen(fpen);
} // end of sd_scale (reset)


int extra_nvarcalc(par_t_reg& param)
{
  int ii = 0;
  for (int k=1; k <= param.get_ngrid(); k++)
  {
    for (int s=1; s<=param.get_nseason(); s++)
    {
      ii ++;
    }
  }
  return(ii);
}

void extra_names(par_t_reg& param,adstring_array& name, const int nvar)
{
  int ii = nvar;
  for (int k=1; k <= param.get_ngrid(); k++)
  {
    for (int s=1; s<=param.get_nseason(); s++)
    {
	    name(++ii) = "DD("+str(s)+","+str(k)+")";
    }
  }
}
