#include "par_t_nn.h"
#include "trace.h"

double dfboundp( double x, double fmin, double fmax, double& g);

extern ofstream clogf;

const extern double min_nb_par;
const extern double max_nb_par;

//const extern double min_slope;
//const extern double max_slope;
//const extern double min_offset;
//const extern double max_offset;

void par_t_nn::dfreset(dvector& g, dvector&  x)
{
  int     ii, k, s;
  double gpen=0;

  ii = 0;

  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      //afm(i) = boundp(x(ii),minafm,maxafm,fpen);
      g(ii) += afm(i)*dfboundp(x(ii),minafm,maxafm,gpen);
      g(ii) += gpen;
    }
  }

  if(m_ipar[22] == 1)
  {
    ii++;
  //param.mort = boundp(x[ii],minmort,maxmort,fpen);
    g[ii] += mort*dfboundp(x[ii],minmort,maxmort, gpen);
    g[ii] += gpen;
    if(m_ipar[19] > 0)
    {
      ii++;
    //param.special_mort = boundp(x[ii],minmort,maxmort,fpen);
      g[ii] += special_mort*dfboundp(x[ii],minmort,maxmort, gpen);
      g[ii] += gpen;
    }
  }

  if(m_ipar[25] == 1)
  {
    ii++;
    for (int f = 1; f <= nfleet; f++)
    {
      //param.q[f] = boundp(x[ii],minq,maxq,fpen);
      g[ii] += q[f]*dfboundp(x[ii],minq,maxq, gpen);
      g[ii] += gpen;
    }
  }

  if(m_ipar[25] == 2)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //param.q[f] = boundp(x[ii],minq,maxq,fpen);
      g[ii] += q[f]*dfboundp(x[ii],minq,maxq, gpen);
      g[ii] += gpen;
    }
  }

  //report_rates
  if(m_ipar[28] == 1)
  {
    //clogf << "***dfreset" << endl;
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //param.report_rates[f] = boundp(x[ii],minr_rates,maxr_rates,fpen);
      g[ii] += report_rates[f]*dfboundp(x[ii],minr_rates,maxr_rates, gpen);
      g[ii] += gpen;
      //clogf << "*->rr:" << report_rates[f] << endl;
    }
  }

  if ( (m_ipar[11]==4) || (m_ipar[11]==14) )
  {
    ii++;
    //nb_par(1) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
    g[ii] += nb_par(1)*dfboundp(x[ii],min_nb_par,max_nb_par,gpen);
    g[ii] += gpen;
    if (m_ipar[11]==14)
    {
      ii++;
      //nb_par(2) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
      g[ii] += nb_par(2)*dfboundp(x[ii],min_nb_par,max_nb_par,gpen);
      g[ii] += gpen;
    }
  }

  if  (m_ipar[11]==24)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //nb_par(f) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
      g[ii] += nb_par(f)*dfboundp(x[ii],min_nb_par,max_nb_par,gpen);
      g[ii] += gpen;
    }
  }

  int nlayer = nn.get_nlayer();
  ivector nrow = nn.get_nrow();
  ivector ncol = nn.get_ncol();

 if( m_ipar[149] == 1)
 {
  // count weights
  d3_array& dfweight = nn.get_weights();  //get df_weights!
  for (int i = 1; i < nlayer; i++)
  {
    for (int j = 1; j <= nrow(i); j ++)
    {
      for (int k = 1; k <= ncol(i); k++)
      {
        ii++;
        //weight(i,j,k) = boundp(x[ii], min_weight, max_weight, fpen);
        g[ii] +=dfweight(i,j,k)*dfboundp(x[ii], min_weight, max_weight, gpen);
        g[ii] += gpen;
      }
    }
  }
} // if(m_ipar[149]==1)


 if( m_ipar[150] == 1)
 {
  // count biases
  dvector& dfbias = nn.get_bias();
  for (int i = 1; i < nlayer; i++)
  {
    ii ++;
    //bias(i) = boundp(x[ii], min_bias, max_bias, fpen);
    g[ii] += dfbias(i)*dfboundp(x[ii], min_bias, max_bias, gpen);
    g[ii] += gpen;
  }
 } // if (m_ipar[150] == 1)

 if( m_ipar[143] == 1)
 {
  // count slope & offset scaling parameters
  ii ++;
  //u_slope = boundp(x(ii), min_slope, max_slope, fpen);
  g(ii) += u_slope*dfboundp(x(ii), min_slope, max_slope, gpen);
  g(ii) += gpen;
  }

 if (m_ipar[144] == 1)
 {
  ii ++;
  //u_offset = boundp(x(ii), min_offset, max_offset, fpen);
  g(ii) += u_offset*dfboundp(x(ii), min_offset, max_offset, gpen);
  g(ii) += gpen;
 }

 if( m_ipar[145] == 1)
 {
    ii ++;
    //v_slope = boundp(x(ii), min_slope, max_slope, fpen);
    g(ii) += v_slope*dfboundp(x(ii), min_slope, max_slope, gpen);
    g(ii) += gpen;
  }

 if( m_ipar[146] == 1)
 {
    ii ++;
    //v_offset = boundp(x(ii), min_offset, max_offset, fpen);
    g(ii) += v_offset*dfboundp(x(ii), min_offset, max_offset, gpen);
    g(ii) += gpen;
 }

 if( m_ipar[147] == 1)
 {
  ii ++;
  //sig_slope = boundp(x(ii), min_slope, max_slope);
  g(ii) += sig_slope*dfboundp(x(ii), min_slope, max_slope, gpen);
  g(ii) += gpen;
 }

 if( m_ipar[148] == 1)
 {
  ii ++;
  //sig_offset = boundp(x(ii), min_offset, max_offset);
  g(ii) += sig_offset*dfboundp(x(ii), min_offset, max_offset, gpen);
  g(ii) += gpen;
 }


} /* end of dfreset */
