//$Id: par_t_reg-reset2.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"
#include "trace.h"

template <>
void par_t_reg<d3_array,dmatrix,dvector,double>::reset(const model_parameters& parameters, dvariable& plike)
{
  HERE
}
template <>
void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::reset(const model_parameters& parameters, dvariable& plike)
{
  HERE
}
