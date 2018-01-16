#include "par_treg.h"
#include "trace.h"

const extern double min_nb_par;
const extern double max_nb_par;

/* puts model x vector for fminim */
double boundp( double x, double fmin, double fmax,double& fpen);

void par_t_reg::reset(dvector& x)
{
  int     ii = 0, k, s;

  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      afm(i) = boundp(x(ii),minafm,maxafm,fpen);
    }
  }

  if(m_ipar[21] == 1)
  {
    ii ++;
    for (k=1; k <= ngrid; k++)
      for (s=1; s <= nseason; s++)
        if (stratum_active(s, k))
	       usergrid[s][k].sigma = boundp(x[ii],minsig,maxsig,fpen);
  }

 
  if(m_ipar[21] == 2)
  {
    for (k=1; k <= ngrid; k++)
      for (s=1; s<= nseason; s++)
        if (stratum_active(s, k))
	     {
	       ii ++;
	       usergrid[s][k].sigma = boundp(x[ii],minsig,maxsig,fpen);
	     }
  }

  if(m_ipar[22] == 1)
  {
    ii++;
    mort = boundp(x[ii],minmort,maxmort,fpen);
    if(m_ipar[19] >0)
    {
      ii++;
      special_mort = boundp(x[ii],minmort,maxmort,fpen);
    }
  }

  if(m_ipar[25] == 1)
  {
    ii++;
    for (int f = 1; f <= nfleet; f++)
    {
      q[f] = boundp(x[ii], minq, maxq, fpen);
    }
  }

  if(m_ipar[25] == 2)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      q[f] = boundp(x[ii],minq,maxq,fpen);
    }
  }

  if(m_ipar[28] == 1)
  {
    //report_rates
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      report_rates[f] = boundp(x[ii],minr_rates,maxr_rates,fpen);
    }
  }

 if(m_ipar[23] == 1)
 {
    for (k=1; k <= ngrid; k++)
    {
      for (s=1; s <= nseason; s++)
      {
        if (stratum_active(s, k))
	     {
	       ii++;
	       usergrid[s][k].u = boundp(x[ii],minvel,maxvel,fpen);

	       ii++;
	       usergrid[s][k].v = boundp(x[ii],minvel,maxvel,fpen);
	     }
      }
    }
  }

  if ( (m_ipar[11]==4) || (m_ipar[11]==14) )
  {
    ii++;
    nb_par(1) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
    if (m_ipar[11]==14)
    {
      ii++;
      nb_par(2) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
    }
  }
 
  if  (m_ipar[11]==24)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //x[ii] = BOUNDPIN(nb_par(f),min_nb_par,max_nb_par);
      nb_par(f) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
    }
  }

}//reset(dvector&)
