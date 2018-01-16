/**
 * $Id: MinimizerTemplate.cpp 2737 2010-08-05 22:14:06Z johnoel $
 */
#include "MinimizerTemplate.h"


//double MinimizerTemplate::minimize(SimpleAdmodel& simpleAdmodel) {
double MinimizerTemplate::minimize(AdmodelTemplate& admodelTemplate) {
	double objectiveFunctionValue = 0;

	const int totalVariables = admodelTemplate.getTotalVariables();

	independent_variables independantVariables(1, totalVariables);
	admodelTemplate.initialize(independantVariables);

	_gradients.deallocate();
	_gradients.allocate(1,totalVariables);
	_gradients.initialize();

	fmm functionMinimizer(totalVariables);
	while (functionMinimizer.ireturn >= 0) {
		functionMinimizer.fmin(objectiveFunctionValue, independantVariables, _gradients);
		if (functionMinimizer.ireturn > 0) {
			objectiveFunctionValue = admodelTemplate.computeObjectiveFunctionValue(independantVariables);
			gradcalc(totalVariables, _gradients);
		}
		admodelTemplate.output();
	}
	return objectiveFunctionValue;
}
