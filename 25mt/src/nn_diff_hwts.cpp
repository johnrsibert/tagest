//$Id: nn_diff_hwts.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "par_t_nn.h"


/* puts the values of diff_hwts back into the weights in the neural network */
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::reset_diff_hwts(neural_net_vector<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn)
{
  const int num_nn = nn.indexmax();
  for(int s = 1; s <= num_nn; s++)
  {
     ivector nrow = nn(s).get_nrow();
     ivector ncol = nn(s).get_ncol();
     int r1 = nrow.indexmax();
     int c1 = ncol.indexmax();
     if(nn(s).nn_ipar(5) )
     {
       D3_ARRAY weights = nn(s).get_weights();
       const int nlayer = nn(s).get_nlayer();
       const MATRIX diff_hwts = nn(s).get_diff_hwts();
       for(int j = 1; j <= nrow(r1); j++)
       {
          weights(nlayer-1,j,1) = diff_hwts(j,1);   //make the first weight = diff_hwts
          for(int k = 2; k <= ncol(c1); k++)
          {
             weights(nlayer-1,j,k) = weights(nlayer-1,j,k-1) + diff_hwts(j,k);
           } // for k
       } //for j
       //clogf <<"wts:  " << nn(s).get_weights() << endl;
       //clogf <<"diff_hwts: " << nn(s).get_diff_hwts() << endl;
     } //if nn(s)
  } // for s
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::reset_diff_hwts(neural_net_vector<d3_array,dmatrix,dvector,double>& nn);
template void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::reset_diff_hwts(neural_net_vector<dvar3_array,dvar_matrix,dvar_vector,dvariable>& nn);

/* calculating derivatives of diff_hwts wrt to weights */
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::dfreset_diff_hwts(neural_net_vector<d3_array,dmatrix,dvector,double>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn)
{
  const int num_nn = nn.indexmax();
  for(int s = 1; s <= num_nn; s++)
  {
     ivector nrow = nn(s).get_nrow();
     ivector ncol = nn(s).get_ncol();
     int r1 = nrow.indexmax();
     int c1 = ncol.indexmax();
     if(nn(s).nn_ipar(5) )
     {
       const d3_array weights = nn(s).get_weights();
       const int nlayer = nn(s).get_nlayer();

       d3_array dfweights = dfnn(s).get_weights();
       dmatrix dfdiff_hwts = dfnn(s).get_diff_hwts();

       for(int j = nrow(r1); j >= 1; j--)
       {
         for(int k = ncol(c1); k >= 2; k--)
         {
           //weights(nlayer-1,j,k) = weights(nlayer-1,j,k-1) + diff_hwts(j,k);
           dfdiff_hwts(j,k) +=    dfweights(nlayer-1,j,k);
           dfweights(nlayer-1,j,k-1) += dfweights(nlayer-1,j,k);
           dfweights(nlayer-1,j,k)=0.0;
         }
          //weights(nlayer-1,j,1) = diff_hwts(j,1);
         dfdiff_hwts(j,1) += dfweights(nlayer-1,j,1);
         dfweights(nlayer-1,j,1) = 0.0;
       }
     }
  }
}
template void par_t_nn<d3_array,dmatrix,dvector,double>::dfreset_diff_hwts(neural_net_vector<d3_array,dmatrix,dvector,double>& nn, neural_net_vector<d3_array,dmatrix,dvector,double>& dfnn);
