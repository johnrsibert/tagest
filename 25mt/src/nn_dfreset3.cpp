//$Id: nn_dfreset3.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "trace.h"
#include "ipar.h"

extern ofstream clogf;

const extern double min_nb_par;
const extern double max_nb_par;

namespace movemod {
	double dfboundp(const double x, const double fmin, const double fmax, double &dfpen);
}
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfreset(dvector& g, const dvector& x)
{
	using movemod::dfboundp;

  int     ii = 0;
  double gpen=0;

  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      //afm(i) = boundp(x(ii),minafm,maxafm,fpen);
      g(ii) = afm(i)*dfboundp(x(ii),minafm,maxafm,gpen);
      g(ii) += gpen;
    }
  }

  if(m_ipar[22] == 1)
  {
    ii++;
  //param.mort = boundp(x[ii],minmort,maxmort,fpen);
    g[ii] = mort*dfboundp(x[ii],minmort,maxmort, gpen);
    g[ii] += gpen;
    if(m_ipar[19] > 0)
    {
      ii++;
    //param.special_mort = boundp(x[ii],minmort,maxmort,fpen);
      g[ii] = special_mort*dfboundp(x[ii],minmort,maxmort, gpen);
      g[ii] += gpen;
    }
  }

  if(m_ipar[25] == 1)
  {
    ii++;
    for (int f = 1; f <= nfleet; f++)
    {
      //param.q[f] = boundp(x[ii],minq,maxq,fpen);
      g[ii] = q[f]*dfboundp(x[ii],minq,maxq, gpen);
      g[ii] += gpen;
    }
  }

  if(m_ipar[25] == 2)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //param.q[f] = boundp(x[ii],minq,maxq,fpen);
      g[ii] = q[f]*dfboundp(x[ii],minq,maxq, gpen);
      g[ii] += gpen;
    }
  }

  //report_rates
  if(m_ipar[28] == 1)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //param.report_rates[f] = boundp(x[ii],minr_rates,maxr_rates,fpen);
      g[ii] = report_rates[f]*dfboundp(x[ii],minr_rates,maxr_rates, gpen);
      g[ii] += gpen;
      //clogf << "*->rr:" << report_rates[f] << endl;
    }
  }

  if ( (m_ipar[11]==4) || (m_ipar[11]==14) )
  {
    ii++;
    //nb_par(1) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
    g[ii] = nb_par(1)*dfboundp(x[ii],min_nb_par,max_nb_par,gpen);
    g[ii] += gpen;
    if (m_ipar[11]==14)
    {
      ii++;
      //nb_par(2) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
      g[ii] = nb_par(2)*dfboundp(x[ii],min_nb_par,max_nb_par,gpen);
      g[ii] += gpen;
    }
  }

  if  (m_ipar[11]==24)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      //nb_par(f) = boundp(x[ii],min_nb_par,max_nb_par,fpen);
      g[ii] = nb_par(f)*dfboundp(x[ii],min_nb_par,max_nb_par,gpen);
      g[ii] += gpen;
    }
  }



  // count slope & offset scaling parameters
 if( m_ipar[71] == 1)
 {
  ii ++;
  //u_slope = boundp(x(ii), min_slope, max_slope, fpen);
  g(ii) = u_slope*dfboundp(x(ii), min_slope, max_slope, gpen);
  g(ii) += gpen;
  }

 if (m_ipar[72] == 1)
 {
  ii ++;
  //u_offset = boundp(x(ii), min_offset, max_offset, fpen);
  g(ii) = u_offset*dfboundp(x(ii), min_offset, max_offset, gpen);
  g(ii) += gpen;
 }

 if( m_ipar[73] == 1)
 {
    ii ++;
    //v_slope = boundp(x(ii), min_slope, max_slope, fpen);
    g(ii) = v_slope*dfboundp(x(ii), min_slope, max_slope, gpen);
    g(ii) += gpen;
  }

 if( m_ipar[74] == 1)
 {
    ii ++;
    //v_offset = boundp(x(ii), min_offset, max_offset, fpen);
    g(ii) = v_offset*dfboundp(x(ii), min_offset, max_offset, gpen);
    g(ii) += gpen;
 }

if( m_ipar[21] == 1)
{
   ii++;
   //uniform_sigma = boundp(x(ii), uniform_sigma, minsig, maxsig);
   g(ii) = uniform_sigma*dfboundp(x(ii), minsig, maxsig, gpen);
   g(ii) += gpen;
}
else
{
   if( m_ipar[75] == 1)
   {
     ii ++;
     //sig_slope = boundp(x(ii), min_slope, max_slope);
     g(ii) = sig_slope*dfboundp(x(ii), min_slope, max_slope, gpen);
     g(ii) += gpen;
   }

   if( m_ipar[76] == 1)
   {
     ii ++;
     //sig_offset = boundp(x(ii), min_offset, max_offset);
     g(ii) = sig_offset*dfboundp(x(ii), min_offset, max_offset, gpen);
     g(ii) += gpen;
   }
}

 //get the weights
 for(int s = 1; s <= num_nn; s++)
 {
   ivector vnrow = nn(s).get_nrow();
   ivector vncol = nn(s).get_ncol();
   if(nn(s).nn_ipar(5) == 1)
   {
    d3_array dfweights = nn(s).get_weights();
    dmatrix dfdiff_hwts = nn(s).get_diff_hwts();
    for(int i = 1; i < num_levels(s); i++)
    {
      if( i < num_levels(s) -1 )
      {
         for(int j = 1; j <= vnrow(i); j++)
         {
           for(int k = 1;  k <= vncol(i); k++)
           {
             ii++; //TTRACE(min_weight,max_weight)
             //weights(i,j,k) = boundp(x[ii], min_weight, max_weight, fpen);
              g[ii] = dfweights(i,j,k)*dfboundp(x[ii], min_weight, max_weight, gpen);
              g[ii] += gpen;
           } //k
         } //j
      } //if (i < num_levels(s) -1
      else
      {
         for(int j = 1; j <= vnrow(i); j++)
         {
           for(int k = 1; k <= vncol(i); k++)
           {
              ii++; //TTRACE(min_diff_hwts, max_diff_hwts)
             //diff_hwts(j,k) = boundp(x[ii], min_diff_hwts, max_diff_hwts, fpen);
             g[ii] = dfdiff_hwts(j,k)*dfboundp(x[ii], min_diff_hwts, max_diff_hwts, gpen);
             g[ii] += gpen;
           } //for k
         } //for j
      } // else
     } //for i
   } //if(s)
 } // for s


 //biases
 for(int s = 1; s <= num_nn; s++)
 {
   ivector vnrow =nn(s).get_nrow();
   if(nn(s).nn_ipar(6))
    {
      dmatrix dfbias = nn(s).get_bias();
       for(int i = 1; i < num_levels(s); i++)
       {
         //min_bias = - nodes_at_layer(s,i);
         //max_bias =  nodes_at_layer(s,i);
         for(int j = 1; j <= vnrow(i); j++)
          {
            ii++;  //TTRACE(min_bias,max_bias)
            // bias(i,j) = boundp(x[ii], min_bias, max_bias, fpen);
             g[ii] = dfbias(i,j)*dfboundp(x[ii], min_bias, max_bias, gpen);
             g[ii] += gpen;
          } //for j
       } //for i
    } // if(nn(s).
  } //for s
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::dfreset(dvector& g, const dvector& x);
