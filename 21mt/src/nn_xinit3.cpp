#include "precomp2.h"
#include "par_t_nn.h"
#include "trace.h"

double min_nb_par =   0.0;     //negative binomial
double max_nb_par = 100.0;	//negative binomial

extern ofstream clogf;

#ifndef __NO_SCALE__
  #define BOUNDPIN boundpin
#else
  #define BOUNDPIN(p, min, max) p
#endif

void par_t_nn::xinit(dvector& x, adstring_array& name)
{
  int     ii = 0, s, k;

  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      x(ii) = BOUNDPIN(afm(i),minafm,maxafm);
      name(ii) = "afm("+str(i)+")";
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

  TRACE(ii)

  int nlayer = nn.get_nlayer();
  ivector nrow = nn.get_nrow();
  ivector ncol = nn.get_ncol();

  if( m_ipar[149] == 1 )
  {
   // count weights
   d3_array weight = nn.get_weights();
   for (int i = 1; i < nlayer; i++)
    {
      for (int j = 1; j <= nrow(i); j ++)
      {
        for (int k = 1; k <= ncol(i); k++)
        {
          ii++;
          x(ii) = BOUNDPIN(weight(i,j,k), min_weight, max_weight);
          name(ii) = "W("+str(i)+","+str(j)+","+str(k)+")";
        }
      }
  }
 } // if( m_ipar[141] == 1  )

  TRACE(ii)

  if( m_ipar[150] == 1 )
  {
    // count biases
    dvector bias = nn.get_bias();
    for (int i = 1; i < nlayer; i++)
    {
      ii ++;
      x(ii) = BOUNDPIN(bias(i),min_bias, max_bias);
      name(ii) = "bias("+str(i)+")";
    }
 } // if( m_ipar[150] == 1 )

 TRACE(ii);

  if( m_ipar[143] == 1 )
  {
   // count slope & offset scaling parameters
   ii ++;
   x(ii) = BOUNDPIN(u_slope, min_slope, max_slope);
   name(ii) = "u_slope";
  }

 if( m_ipar[144] == 1)
 {
    ii ++;
    x(ii) = BOUNDPIN(u_offset, min_offset, max_offset);
    name(ii) = "u_offset";
 }

  if( m_ipar[145] == 1 )
  {
    ii ++;
    x(ii) = BOUNDPIN(v_slope, min_slope, max_slope);
    name(ii) = "v_slope" ;
 }

 if( m_ipar[146] == 1 )
 {
    ii ++;
    x(ii) = BOUNDPIN(v_offset, min_offset, max_offset);
    name(ii) = "v_offset";
 }

 if( m_ipar[147] == 1)
 {
    ii ++;
    x(ii) = BOUNDPIN(sig_slope, min_slope, max_slope);
    name(ii) = "sig_slope";
 }


 if( m_ipar[148] == 1 )
 {
    ii ++;
    x(ii) = BOUNDPIN(sig_offset, min_offset, max_offset);
    name(ii) = "sig_offset";
 }

  TRACE(ii);


}//xinit
