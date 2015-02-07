//$Id: par_t_reg-computeObjectiveFunctionValue.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"

template <>
dvariable par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::computeObjectiveFunctionValue(model_parameters& parameters)
{
	return 0;
}

template <>
dvariable par_t_reg<d3_array,dmatrix,dvector,double>::computeObjectiveFunctionValue(model_parameters& parameters)
{
	return 0;
}
