//$Id: nn_weightpen.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "par_t_nn.h"
/*
void  weight_penalty(dvector& wts, const double wts_weight, double& wts_penalty)
{
  wts_penalty = norm2(wts) * wts_weight;
}


void dfweight_penalty( dvector& wts, dvector& dfwts, const double wts_weight, double& dfwts_penalty)
{
  //wts_penalty = norm2(wts) * wts_weight;
  dfwts += dfwts_penalty * wts_weight * 2.0*sum(wts);
  dfwts_penalty = 0.0;
}
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void weight_penalty(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, const double wts_weight, DOUBLE& wts_penalty, const int m_ipar78)
{
   const int num_nn = nn.indexmax();
   for(int s = 1; s <= num_nn; s++)
   {
     if(nn(s).nn_ipar(5))
     {
		ivector vnrow = nn(s).get_nrow();
		ivector vncol  = nn(s).get_ncol();
       const D3_ARRAY weights = nn(s).get_weights();
       const int num_levels = nn(s).get_nlayer();
       for(int i = 1; i <  num_levels; i++)
        {
           for(int j  =1; j  <= vnrow(i); j++)
           {
             for(int k = 1; k <= vncol(i); k++)
             {
               if( (m_ipar78 == 1) && (i ==1) )
                {
                  wts_penalty += wts_weight*weights(i,j,k)*weights(i,j,k);
                }
                else if ( (m_ipar78 == 2) && (i < (num_levels - 1)) )
               {
                 wts_penalty += wts_weight*weights(i,j,k)*weights(i,j,k);
               }

             } //for k
           } //for j
        } //for i
      } //if(s)
   } //for s
}
template void weight_penalty<d3_array,dmatrix,dvector,double>(neural_net_vector<d3_array,dmatrix,dvector,double>& nn, const double wts_weight, double& wts_penalty, const int m_ipar78);
template void weight_penalty<dvar3_array,dvar_matrix,dvar_vector,dvariable>(neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& nn, const double wts_weight, dvariable& wts_penalty, const int m_ipar78);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfweight_penalty(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn, const double wts_weight, double& dfwts_penalty, const int m_ipar78)
{
   const int num_nn = nn.indexmax();

   for(int s = num_nn; s >= 1; s--)
   {
     ivector vnrow = nn(s).get_nrow();
     ivector vncol = nn(s).get_ncol();
     //TRACE(s)
     //TRACE(vnrow)
     //TRACE(vncol)
     if(nn(s).nn_ipar(5))
     {
       const d3_array weights = nn(s).get_weights();
       d3_array dfweights = dfnn(s).get_weights();
       const int num_levels = nn(s).get_nlayer();
       for(int i = num_levels-1; i >=  1; i--)
       {
           for(int j  =vnrow(i); j  >= 1; j--)
           {
             for(int k = vncol(i); k >= 1; k--)
             {
               if( (m_ipar78 == 1) && (i ==1) )
               {
                 //wts_penalty += wts_weight*weights(i,j,k)*weights(i,j,k);
                 dfweights(i,j,k) += dfwts_penalty*2.0*wts_weight*weights(i,j,k);
              }
              else if ( (m_ipar78 == 2) && (i < (num_levels - 1)) )
              {
                //wts_penalty += wts_weight*weights(i,j,k)*weights(i,j,k);
                dfweights(i,j,k) += dfwts_penalty*2.0*wts_weight*weights(i,j,k);
               }

             } //for k
           } //for j
        } //for i
      } //if(s)
   } //for s
}
template void dfweight_penalty(neural_net_vector<d3_array,dmatrix,dvector,double>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn, const double wts_weight, double& dfwts_penalty, const int m_ipar78);
template<> void dfweight_penalty(neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn, const double wts_weight, double& dfwts_penalty, const int m_ipar78)
{
}
