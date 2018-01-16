//$Id: nn_dfoutput_comp.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "neural_net.h"
#include "trace.h"

void dfsigmoid_squash_func(const dvector& v, dvector& dfv);
void dflogistic_squash_func(const dvector& v, dvector& dfv);
void dflinear_squash_func(const dvector& v, dvector& dfv);

//function defined as a friend function in the nueral_net class
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfoutput_comp(neural_net<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& nn, neural_net<d3_array,dmatrix,dvector,double>& dfnn, dvector& dfoutput, const int squash_flag)
{
  // temporary variables to dererence member variables
  const int nlayer = nn.get_nlayer();
  const ivector nrow = nn.get_nrow();
  const ivector ncol = nn.get_ncol();

  dmatrix& nodes = nn.nodes;
  //dvector& bias = nn.bias;
  dmatrix& bias = nn.bias;
  d3_array& weights = nn.weights;

  dmatrix& dfnodes = dfnn.nodes;     dfnodes.initialize();
  //dvector& dfbias = dfnn.bias;
  dmatrix& dfbias = dfnn.bias;
  d3_array& dfweights = dfnn.weights;

  //return output;
  //dfoutput already has the return value!

  //output = nodes(nlayer);
   dfnodes(nlayer) += dfoutput;
   dfoutput.initialize();

  for(int i = nlayer-1; i >=1 ; i--)
  {
   // squash_func(nodes[i+1]);  //apply the scaling function to each step forward

   if( (squash_flag == 1) &&  (i < nlayer-1)  )
        dfsigmoid_squash_func(nodes( i+1), dfnodes( i+1) );
   else if( (squash_flag == 2) &&  (i < nlayer-1) )
       dflinear_squash_func(nodes( i+1), dfnodes( i+1) );
   else if( (squash_flag == 3 ) &&  (i < nlayer-1) )
    dflogistic_squash_func(nodes( i+1), dfnodes(i+1) );

    //dfsigmoid_squash_func(nodes( i+1), dfnodes( i+1) );

    for(int j = nrow(i); j >=1 ; j--)
    {
     //nodes(i+1,j) += bias(i,j);
      dfbias(i,j) += dfnodes(i+1,j);

      for(int k = ncol(i); k >= 1; k--)
      {
        //nodes(i+1,j) += weights(i,j,k) * nodes(i,k);
        dfweights(i,j,k) += dfnodes(i+1,j) * nodes(i,k);
        dfnodes(i,k)      += dfnodes(i+1,j) * weights(i,j,k);
      }
      //nodes(i+1,j) = 0.0;
      dfnodes(i+1,j) = 0.0;
    }

  }

  //nodes(1) = input;
  dfnodes(1).initialize();
  //clogf <<__FILE__<<":"<<dfoutput << endl;
}
template void dfoutput_comp(neural_net<d3_array,dmatrix,dvector,double>& nn, neural_net<d3_array,dmatrix,dvector,double>& dfnn, dvector& dfoutput, const int squash_flag);
template<> void dfoutput_comp(neural_net<dvar3_array,dvar_matrix,dvar_vector,dvariable>& nn, neural_net<d3_array,dmatrix,dvector,double>& dfnn, dvector& dfoutput, const int squash_flag)
{
}


void dfsigmoid_squash_func(const dvector& v, dvector& dfv)
{
  //returns the nodes in the range -0.5 and +0.5
  int min = v.indexmin();
  int max = v.indexmax();
  double dftmp = 0.0;

  for(int k = max; k >= min; k--)
  {
    //v(k) = tmp;
    dftmp += dfv(k);
    dfv(k) = 0.0;

      double tv = tan( v(k) * M_PI );  //unsquash v(k)
     //double tmp = (1.0/M_PI) * atan( v(k) )

     dfv(k) = dftmp*( 1.0/ M_PI) / (  1.0 + ( tv * tv )   );
     dftmp = 0.0;
  }
}

void dflogistic_squash_func(const dvector& v, dvector& dfv)
 {
    int min = v.indexmin();
    int max = v.indexmax();
    double dftmp = 0.0;

    for(int k = max; k >= min; k--)
    {
       //v(k) = tmp;
       dftmp += dfv(k);
       dfv(k) = 0.0;

       double tv = - log((1.0 - v(k)) /v(k) );  //unsquash
       //tmnp = 1.0 (1.0 + exp(-v(k)) );

       dfv(k) = dftmp * exp(tv) / (   (1.0 + exp(- tv)) * (1.0 + exp(- tv))  );
       dftmp = 0.0;

    }
    cout <<"logisitic function not working properly... " << endl;
    cout <<"Bailing out in " << __FILE__<<",line# "<<__LINE__ << endl;
    exit(1);

 }



void dflinear_squash_func(const dvector& v, dvector& dfv)
{
  //returns the nodes in the range -0.5 and +0.5
  int min = v.indexmin();
  int max = v.indexmax();
  double dftmp = 0.0;

  for(int k = max; k >= min; k--)
  {
    //v(k) = tmp;
    dftmp += dfv(k);
    dfv(k) = 0.0;

    double tv =  ( v(k) - 1.0)  / 0.05;  //unsquash
     // tmp = (0.05 * v(k) ) * 1.0;
     dfv(k) =  dftmp *  0.05;
      dftmp = 0.0;

  }
}
