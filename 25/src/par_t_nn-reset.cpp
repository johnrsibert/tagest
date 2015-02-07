//$Id: par_t_nn-reset.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_nn.h"

template <>
void par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>::reset(const model_parameters& parameters, dvariable& plike)
{
}

template <>
void par_t_nn<d3_array,dmatrix,dvector,double>::reset(const model_parameters& parameters, dvariable& plike)
{
}
