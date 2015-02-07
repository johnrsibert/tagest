//$Id: nn_switchpen.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "par_t_nn.h"
#include <math.h>


/* Ignor this equation
double swpen(double w1, double w2, const double penalty )
{
   double ret;
   ret  = (penalty *pow(-0.5*((w2-w1)-sfabs(w2-w1)),2));
   return (ret);
}
*/


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void weight_switch_penalty(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, const double switch_wt, DOUBLE& wts_switch_penalty)
 {
   const int num_nn = nn.indexmax();
   for(int s = 1; s <= num_nn; s++)
   {
     ivector vnrow = nn(s).get_nrow();
     ivector vncol = nn(s).get_ncol();
     if(nn(s).nn_ipar(5))
     {
         const D3_ARRAY weights  = nn(s).get_weights();
         const int num_levels = nn(s).get_nlayer();
         for(int j = 1; j <= vnrow(num_levels-1); j++)
         {
          for(int k = 2; k <= vncol(num_levels-1); k++)
           {
              if( weights(num_levels-1,j,k-1) >  weights(num_levels-1,j,k) )
                {
                  DOUBLE diff = ( weights(num_levels-1,j,k-1) - weights(num_levels-1,j,k) );
                  wts_switch_penalty += switch_wt *  (diff*diff);
                } //if
            } //for k
          } //for j
      }//if(s)
   }//fors
 }
template void weight_switch_penalty(neural_net_vector<d3_array,dmatrix,dvector,double>& nn, const double switch_wt, double& wts_switch_penalty);
template void weight_switch_penalty(neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& nn, const double switch_wt, dvariable& wts_switch_penalty);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfweight_switch_penalty(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn, const double switch_wt, double& dfsw_pen)
{
   const int num_nn  = nn.indexmax();
   for(int s = 1; s <= num_nn; s++)
   {
     ivector vnrow = nn(s).get_nrow();
     ivector vncol = nn(s).get_ncol();
     if(nn(s).nn_ipar(5))
     {
        const d3_array weights = nn(s).get_weights();
        d3_array dfweights = dfnn(s).get_weights();
        const int num_levels = nn(s).get_nlayer();
        for(int j = vnrow(num_levels-1); j >= 1; j--)
        {
           for(int k = vncol(num_levels-1); k >= 2; k--)
           {
             if(weights(num_levels-1,j,k-1) > weights(num_levels-1,j,k) )
              {
                //recalculate
                 double diff = ( weights(num_levels-1,j,k-1) - weights(num_levels-1,j,k) );

                //wts_switch_penalty += switch_wt *  (diff*diff);
                 double dfdiff = dfsw_pen * 2.0* switch_wt* diff;

                 //double diff = ( weights(num_levels-1,j,k-1) - weights(num_levels-1,j,k) );
                  dfweights(num_levels-1,j,k-1) += dfdiff;
                  dfweights(num_levels-1,j,k)  += dfdiff;
                  dfdiff = 0.0;

             } // if
           } //for j
        } //for i
      } //if(s)
    } //for s
}
template void dfweight_switch_penalty(neural_net_vector<d3_array,dmatrix,dvector,double>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn, const double switch_wt, double& dfsw_pen);
template<> void dfweight_switch_penalty(neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn, const double switch_wt, double& dfsw_pen)
{
}
