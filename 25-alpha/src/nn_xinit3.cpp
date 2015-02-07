//$Id: nn_xinit3.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"
#include "trace.h"

extern double min_nb_par;//negative binomial
extern double max_nb_par;//negative binomial

extern ofstream clogf;

namespace movemod {
	double boundpin(const double x, const double fmin, const double fmax);
}
template<> void par_t_nn<d3_array,dmatrix,dvector,double>::xinit(dvector& x, adstring_array& name)
{
	using movemod::boundpin;

  int     ii = 0;

  if (m_ipar[24] > 0)
  {
    for (int i = 1; i <= num_afm; i++)
    {
      ii++;
      x(ii) = boundpin(afm(i),minafm,maxafm);
      name(ii) = "afm("+str(i)+")";
    }
  }


  if(m_ipar[22] == 1)
  {
      ii++;
      x[ii] = boundpin(mort, minmort, maxmort);
      name(ii) = "M";
      if(m_ipar[19] > 0)
      {
        ii++;
        x[ii] = boundpin(special_mort, minmort, maxmort);
        name(ii) = "M("+str((int)m_ipar[19])+")";
      }
  }

  //uniform q
  if(m_ipar[25] == 1)
  {
      ii++;
      x[ii] = boundpin(q(1), minq, maxq);
      name(ii) = "q";
  }

  //separate q for each fleet
  if(m_ipar[25] == 2)
  {
      for (int f = 1; f <= get_nfleet(); f++)
      {
          ii++;
          x[ii] = boundpin(q(f), minq, maxq);
          name(ii) = "q("+str(f)+")";
      }
  }

  if(m_ipar[28] == 1)
  {
      //separate report_rates for each fleet
      for (int f = 1; f <= nfleet; f++)
      {
          ii++;
          x[ii] = boundpin(report_rates[f], minr_rates, maxr_rates);
          clogf << "*->p.rr::" << report_rates[f] << endl;
          clogf << "x[ii]::" << x[ii] << endl;
          name(ii) = "rr("+str(f)+")";
      }
  }


  if ( (m_ipar[11]==4) || (m_ipar[11]==14) )
  {
    ii++;
    x[ii] = boundpin(nb_par(1),min_nb_par,max_nb_par);
    name(ii) = "NB-par(1)";
    if (m_ipar[11]==14)
    {
      ii++;
      x[ii] = boundpin(nb_par(2),min_nb_par,max_nb_par);
      name(ii) = "NB-par(2)";
    }
  }

  if  (m_ipar[11]==24)
  {
    for (int f = 1; f <= nfleet; f++)
    {
      ii++;
      x[ii] = boundpin(nb_par(f),min_nb_par,max_nb_par);
      name(ii) = "NB-par("+str(f)+")";
    }
  }

  TRACE(ii)

  if( m_ipar[71] == 1 )
  {
   // count slope & offset scaling parameters
   ii ++;
   x(ii) = boundpin(u_slope, min_slope, max_slope);
   name(ii) = "u_slope";
  }

 if( m_ipar[72] == 1)
 {
    ii ++;
    x(ii) = boundpin(u_offset, min_offset, max_offset);
    name(ii) = "u_offset";
 }

  if( m_ipar[73] == 1 )
  {
    ii ++;
    x(ii) = boundpin(v_slope, min_slope, max_slope);
    name(ii) = "v_slope" ;
 }

 if( m_ipar[74] == 1 )
 {
    ii ++;
    x(ii) = boundpin(v_offset, min_offset, max_offset);
    name(ii) = "v_offset";
 }

 if( m_ipar[21] == 1)
 {
   ii++;
   x(ii) = boundpin(uniform_sigma, minsig, maxsig);
   name(ii) = "uniform_sigma";
 }
 else
 {
   if( m_ipar[75] == 1)
   {
      ii ++;
      x(ii) = boundpin(sig_slope, min_slope, max_slope);
      name(ii) = "sig_slope";
   }

   if( m_ipar[76] == 1 )
   {
      ii ++;
      x(ii) = boundpin(sig_offset, min_offset, max_offset);
      name(ii) = "sig_offset";
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
    dmatrix diff_hwts  = nn(s).get_diff_hwts();
    for(int i = 1; i < num_levels(s); i++)
    {
      if( i < num_levels(s) -1 )
      {
         for(int j = 1; j <= vnrow(i); j++)
         {
           for(int k = 1;  k <= vncol(i); k++)
           {
             ii++; //TTRACE(min_weight, max_weight)
             x(ii) = boundpin(weights(i,j,k), min_weight, max_weight);
             name(ii) = "W_N"+ str(s) +" ("+ str(i)+","+str(j)+","+str(k)+")";
           } //k
         } //j
      } //if (i < num_levels(s) -1
      else
      {
         for(int j = 1; j <= vnrow(i); j++)
         {
           for(int k = 1; k <= vncol(i); k++)
           {
              ii++; //TTRACE(min_diff_hwts,max_diff_hwts)
             x(ii) = boundpin(diff_hwts(j,k), min_diff_hwts, max_diff_hwts);
             name(ii) = "diff_hwts"+ str(s) +" ("+ str(i)+","+str(j)+","+str(k)+")";
           } //for k
         } //for j
      } // else
     } //for i
   } //if(s)
 } // for s



  //biases
 for(int s = 1; s <= num_nn; s++)
 {
   if(nn(s).nn_ipar(6) )
    {
   ivector vnrow = nn(s).get_nrow();
       dmatrix bias = nn(s).get_bias();
       for(int i = 1; i < num_levels(s); i++)
       {
         //min_bias = - nodes_at_layer(s,i);
         //max_bias =  nodes_at_layer(s,i);
         TTRACE(min_bias,max_bias)
          for(int j = 1; j <= vnrow(i); j++)
          {
            ii++;  //TTRACE(min_bias,max_bias)
            x(ii) = boundpin(bias(i,j),min_bias, max_bias);
            name(ii) = "bias_N" + str(s) +" (" + str(i)+","+str(j)+")";
          }//for j
       } //for i
    } // if(nn(s).
  } //for s
}
template<> void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::xinit(dvector& x, adstring_array& name) {}
