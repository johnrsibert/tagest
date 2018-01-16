#include "par_t_nn.h"
#include "trace.h"

const extern double min_nb_par;
const extern double max_nb_par;
//const extern double min_slope;
//const extern double max_slope;
//const extern double min_offset;
//const extern double max_offset;

/* puts model x vector for fminim */
double boundp( double x, double fmin, double fmax,double& fpen);

void par_t_nn::reset(dvector& x)
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

  int nlayer = nn.get_nlayer();
  ivector nrow = nn.get_nrow();
  ivector ncol = nn.get_ncol();

  if( m_ipar[149] == 1 )
  {
  // count weights
  d3_array& weight = nn.get_weights();
  for (int i = 1; i < nlayer; i++)
  {
    for (int j = 1; j <= nrow(i); j ++)
    {
      for (int k = 1; k <= ncol(i); k++)
      {
        ii++;
        //x(ii) = BOUNDPIN(weight(i,j,k), min_weight, max_weight);
        weight(i,j,k) = boundp(x[ii], min_weight, max_weight, fpen);
      }
    }
  }
 }  // if( m_ipar[141] == 1  )



 if( m_ipar[150] == 1 )
  {
  // count biases
  dvector& bias = nn.get_bias();
  for (int i = 1; i < nlayer; i++)
  {
    ii ++;
    //x(ii) = BOUNDPIN(bias(i),min_bias, max_bias);
    bias(i) = boundp(x[ii], min_bias, max_bias, fpen);
  }
 } // if( m_ipar[150] == 1 )


 if( m_ipar[143] == 1 )
  {
    // count slope & offset scaling parameters
    ii ++;
    //x(ii) = BOUNDPIN(u_slope, min_slope, max_slope);
    u_slope = boundp(x(ii), min_slope, max_slope, fpen);
   }

 if( m_ipar[144] == 1 )
 {
    ii ++;
    //x(ii) = BOUNDPIN(u_offset, min_offset, max_offset);
    u_offset = boundp(x(ii), min_offset, max_offset, fpen);
  }

 if( m_ipar[145] == 1 )
 {
    ii ++;
    //x(ii) = BOUNDPIN(v_slope, min_slope, max_slope);
    v_slope = boundp(x(ii), min_slope, max_slope, fpen);
 }


if( m_ipar[146] == 1 )
 {
    ii ++;
    //x(ii) = BOUNDPIN(v_offset, min_offset, max_offset);
    v_offset = boundp(x(ii), min_offset, max_offset, fpen);
 }

if( m_ipar[147] == 1 )
{
  ii ++;
  //x(ii) = BOUNDPIN(sig_slope, min_slope, max_slope);
  sig_slope = boundp(x(ii), min_slope, max_slope, fpen);
 }

 if( m_ipar[148] == 1)
 {
  ii ++;
  //x(ii) = BOUNDPIN(sig_offset, min_offset, max_offset);
  sig_offset = boundp(x(ii), min_offset, max_offset, fpen);
 }

 }//reset(dvector&)
