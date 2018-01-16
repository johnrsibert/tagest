#include "precomp2.h"
#include "par_treg.h"
#include "trace.h"

double min_nb_par =   0.0;
double max_nb_par = 100.0;

extern ofstream clogf;

#ifndef __NO_SCALE__
  #define BOUNDPIN boundpin
#else
  //jca     
  //------> WARNING <---------
  //bds class was eliminated 1.22.97
  #define BOUNDPIN(p, min, max) p
#endif

void par_t_reg::xinit(dvector& x, adstring_array& name)
{
  int     ii = 0, s, k;
  imatrix& s_a = stratum_active;

  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      x(ii) = BOUNDPIN(afm(i),minafm,maxafm);
     name(ii) = "afm("+str(i)+")";
     }
  }

  if(m_ipar[21] == 1)
  {
      ii ++;
      x[ii] = BOUNDPIN(usergrid[1][1].sigma, minsig, maxsig);
      name(ii) = "sigma";
  }

  if(m_ipar[21] == 2)
  {
      for (k=1; k <= ngrid; k++)
      {
          for (s=1; s <= nseason; s++)
          {
              if (s_a(s, k))
             {
                  ii++;
                 x[ii] = BOUNDPIN(usergrid[s][k].sigma, minsig, maxsig);
                 name(ii) = "D("+str(s)+","+str(k)+")";
              }
          }
      }
  }

  if(m_ipar[22] == 1)
  {
      ii++;
      x[ii] = BOUNDPIN(mort, minmort, maxmort);
      name(ii) = "M";
      if(m_ipar[19] > 0)
      {
        ii++;
        x[ii] = BOUNDPIN(special_mort, minmort, maxmort);
        name(ii) = "M("+str((int)m_ipar[19])+")";
      }
  }

  //uniform q
  if(m_ipar[25] == 1)
  {
      ii++;
      x[ii] = BOUNDPIN(q(1), minq, maxq);
      name(ii) = "q";
  }

  //separate q for each fleet
  if(m_ipar[25] == 2)
  {
      for (int f = 1; f <= get_nfleet(); f++)
      {
          ii++;
          x[ii] = BOUNDPIN(q(f), minq, maxq);
          name(ii) = "q("+str(f)+")";
      }
  }

  //clogf << "***I'm in xinit" << endl;
  if(m_ipar[28] == 1)
  {
      //separate report_rates for each fleet
      for (int f = 1; f <= nfleet; f++)
      {
          ii++;
          x[ii] = BOUNDPIN(report_rates[f], minr_rates, maxr_rates);
          clogf << "*->p.rr::" << report_rates[f] << endl;
          clogf << "x[ii]::" << x[ii] << endl;
          name(ii) = "rr("+str(f)+")";
      }
  }

  if (m_ipar[23]==1)
  {
      for (k = 1; k <= ngrid; k++)
      {
          for (s = 1; s <= nseason; s++)
          {
             if (s_a(s, k))
            {
              ii++;
              x[ii] = BOUNDPIN(usergrid[s][k].u, minvel, maxvel);
              name(ii) = "u("+str(s)+","+str(k)+")";

              ii++;
              x[ii] = BOUNDPIN(usergrid[s][k].v, minvel, maxvel);
              name(ii) = "v("+str(s)+","+str(k)+")";
            }
          }
      }
  }

  if ( (m_ipar[11]==4) || (m_ipar[11]==14) )
  {
    ii++;
    x[ii] = BOUNDPIN(nb_par(1),min_nb_par,max_nb_par);
    name(ii) = "NB-par(1)";
    if (m_ipar[11]==14)
    {
      ii++;
      x[ii] = BOUNDPIN(nb_par(2),min_nb_par,max_nb_par);
      name(ii) = "NB-par(2)";
    }
  } 

  if  (m_ipar[11]==24)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      x[ii] = BOUNDPIN(nb_par(f),min_nb_par,max_nb_par);
      name(ii) = "NB-par("+str(f)+")";
    }
  }
}//xinit
