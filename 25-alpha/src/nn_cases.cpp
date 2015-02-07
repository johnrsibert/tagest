//$Id: nn_cases.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include <fvar.hpp>
#include "trace.h"


template <typename MATRIX, typename VECTOR>
VECTOR nn_cases(int num_nn, const MATRIX& output_layer)
{
  VECTOR nn_output(1,3);  //for u, v, and sigma AND in THIS SEQUENCE ONLY!

   switch(num_nn)
    {
       case 1:
       {
         nn_output(1) = output_layer(1,1);
         //nn_output(2) = output_layer(1,2);
         nn_output(2) = output_layer(2,1);
//         nn_output(3) = output_layer(1,3);
       }
       break;

/*
       case 2:
       {
        HERE
cout << __FILE__ << ':' << __LINE__ << endl;
         nn_output(1) = output_layer(1,1);
         nn_output(2) = output_layer(1,2);
         nn_output(3) = output_layer(2,1);
       }
       break;

       case 3:
       {
         HERE
cout << __FILE__ << ':' << __LINE__ << endl;
         nn_output(1) = output_layer(1,1);
         nn_output(2)  = output_layer(2,1);
         nn_output(3)  = output_layer(3,1);
        }
       break;
*/

    } // switch(num_nn)

  return nn_output;

} //void par_t_nn::nn_cases(int num_nn)
template dvector nn_cases<dmatrix,dvector>(int num_nn, const dmatrix& output_layer);
template dvar_vector nn_cases<dvar_matrix,dvar_vector>(int num_nn, const dvar_matrix& output_layer);


//for reversing the above operation

void nn_dfcases(int num_nn, dvector nn_dfoutput, dmatrix ddfoutput_layer)
{

  //cout <<__FILE__<<":"<<__LINE__<< nn_dfoutput <<endl;
  //cout <<__FILE__<<":"<<__LINE__<<dfoutput_layer << endl;
  switch(num_nn)
  {
    case 1:
    {
      ddfoutput_layer(1,1) = nn_dfoutput(1);
      ddfoutput_layer(1,2) = nn_dfoutput(2);
      //ddfoutput_layer(1,3) = nn_dfoutput(3);
    }
    break;

    case 2:
    {
      ddfoutput_layer(1,1) = nn_dfoutput(1);
      ddfoutput_layer(1,2) = nn_dfoutput(2);
      ddfoutput_layer(2,1) = nn_dfoutput(3);
    }
    break;

    case 3:
    {
      ddfoutput_layer(1,1) = nn_dfoutput(1);
      ddfoutput_layer(2,1) = nn_dfoutput(2);
      ddfoutput_layer(3,1) = nn_dfoutput(3);
    }
    break;

  }
 // cout <<__FILE__<<":"<<__LINE__<<ddfoutput_layer << endl;

}
