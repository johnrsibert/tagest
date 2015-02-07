#include <fvar.hpp>
#include "neural_net.h"
#include "trace.h"

void dfsigmoid_squash_func(const dvector& v, dvector& dfv);
void dflinear_squash_func(const dvector& v, dvector& dfv);

//function defined as a friend function in the nueral_net class
void dfoutput_comp(neural_net& nn, neural_net& dfnn, dvector& dfoutput, const int squash_flag)
{
  // temporary variables to dererence member variables
  const int nlayer = nn.get_nlayer();
  const ivector nrow = nn.get_nrow();
  const ivector ncol = nn.get_ncol();
 
  dmatrix& nodes = nn.nodes;          
  dvector& bias = nn.bias;                  
  d3_array& weights = nn.weights;   
  
  dmatrix& dfnodes = dfnn.nodes;          dfnodes.initialize();       
  dvector& dfbias = dfnn.bias;                
  d3_array& dfweights = dfnn.weights;  
 
  //return output;
  //dfoutput already has the return value!

  //output = nodes(nlayer);
   dfnodes(nlayer) += dfoutput;  
   dfoutput.initialize();

  for(int i = nlayer-1; i >=1 ; i--)
  { 

   //squash_func(nodes[i+1]);  //apply the scaling function to each step forward
   if( squash_flag == 1)
        dfsigmoid_squash_func(nodes( i+1), dfnodes( i+1) );
   else if (squash_flag == 2)
       dflinear_squash_func(nodes( i+1), dfnodes( i+1) );

    for(int j = nrow(i); j >=1 ; j--)
    {
     //nodes(i+1,j) += bias(i);
      dfbias(i) += dfnodes(i+1,j);
           
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

     //double tv = tan( v(k) * M_PI );  //unsquash v(k)
      double tv = tan( v(k));  
     //double tmp = (1.0/M_PI) * atan( v(k) )
     //dfv(k) = dftmp*( 1.0/ M_PI) / (  1.0 + ( tv * tv )   );
      dfv(k) = dftmp  / ( 1.0 + (tv*tv) );
      
    dftmp = 0.0;
  } 
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