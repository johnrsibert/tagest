//$Id: nn_reset3.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "trace.h"

const extern double min_nb_par;
const extern double max_nb_par;

/* puts model x vector for fminim */
namespace movemod {
	double boundp(const double x, const double fmin, const double fmax, double &fpen);
}
template<> void par_t_nn<d3_array,dmatrix,dvector,double>::reset(const dvector& x)
{
	using movemod::boundp;
  int     ii = 0;

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


   //scaling parameters for Neural_network(s)
   if( m_ipar[71] == 1 )
  {
    // count slope & offset scaling parameters
    ii ++;
    //x(ii) = BOUNDPIN(u_slope, min_slope, max_slope);
    u_slope = boundp(x(ii), min_slope, max_slope, fpen);
   }

 if( m_ipar[72] == 1 )
 {
    ii ++;
    //x(ii) = BOUNDPIN(u_offset, min_offset, max_offset);
    u_offset = boundp(x(ii), min_offset, max_offset, fpen);
  }

 if( m_ipar[73] == 1 )
 {
    ii ++;
    //x(ii) = BOUNDPIN(v_slope, min_slope, max_slope);
    v_slope = boundp(x(ii), min_slope, max_slope, fpen);
 }


if( m_ipar[74] == 1 )
 {
    ii ++;
    //x(ii) = BOUNDPIN(v_offset, min_offset, max_offset);
    v_offset = boundp(x(ii), min_offset, max_offset, fpen);
 }

if (m_ipar[21] == 1)
{
   ii++;
   //x(ii) = BOUNDPIN(uniform_sigma, minsig, maxsig);
   uniform_sigma = boundp(x(ii), minsig, maxsig, fpen);
}
else
{
  if( m_ipar[75] == 1 )
  {
    ii ++;
    //x(ii) = BOUNDPIN(sig_slope, min_slope, max_slope);
    sig_slope = boundp(x(ii), min_slope, max_slope, fpen);
   }

   if( m_ipar[76] == 1)
   {
    ii ++;
    //x(ii) = BOUNDPIN(sig_offset, min_offset, max_offset);
    sig_offset = boundp(x(ii), min_offset, max_offset, fpen);
   }
}

 //get the weights
 for(int s = 1; s <= num_nn; s++)
 {
   ivector vnrow = nn(s).get_nrow();
   ivector vncol = nn(s).get_ncol();
   if(nn(s).nn_ipar(5) == 1)
   {
    d3_array weights = nn(s).get_weights();
    dmatrix diff_hwts = nn(s).get_diff_hwts();
    for(int i = 1; i < num_levels(s); i++)
    {
      if( i < num_levels(s) -1 )
      {
         for(int j = 1; j <= vnrow(i); j++)
         {
           for(int k = 1;  k <= vncol(i); k++)
           {
             ii++; //TTRACE(min_weight, max_weight)
             //x(ii) = boundpin(weights(i,j,k), min_weight, max_weight);
             weights(i,j,k) = boundp(x[ii], min_weight, max_weight, fpen);
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
             //x(ii) = BOUNDPIN(diff_hwts(j,k), min_diff_hwts, max_diff_hwts);
             diff_hwts(j,k) = boundp(x[ii], min_diff_hwts, max_diff_hwts, fpen);
           } //for k
         } //for j
      } // else
     } //for i
   } //if(s)
 } // for s



 //biases
 for(int s = 1; s <= num_nn; s++)
 {
   if(nn(s).nn_ipar(6))
    {
   ivector vnrow = nn(s).get_nrow();
      dmatrix bias = nn(s).get_bias();
       for(int i = 1; i < num_levels(s); i++)
       {
         //min_bias = - nodes_at_layer(s,i);
         //max_bias =  nodes_at_layer(s,i);
         for(int j = 1; j <= vnrow(i); j++)
         {
            ii++; //TTRACE(min_bias,max_bias)
            //x(ii) = boundpin(bias(i,j),min_bias, max_bias);
            bias(i,j) = boundp(x[ii], min_bias, max_bias, fpen);
          } //for j
       } //for i
    } // if(nn(s).
  } //for s
}
template<> void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::reset(const dvar_vector& x) {}
