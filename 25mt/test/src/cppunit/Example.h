#ifndef __Example_h__
#define __Example_h__
/**
 * $Id: Simple.h 777 2006-03-11 01:38:06Z johnoel $
 */
#include "AdmodelTemplate.h"

class Example: public AdmodelTemplate
{
public:
Example():_x(0){}

public:
int getTotalVariables() {
	return 1;
}
void initialize(dvector& independantVariables) {
	independantVariables(1) = boundpin(10, -100, 100);
}
double computeObjectiveFunctionValue(dvar_vector independantVariables) {
	dvariable objectiveFunctionValue = -100;
	dvariable penalty = 0;
	dvariable x = boundp(independantVariables(1),-100,100,penalty);
	_x = value(x);
	x += 5;
	objectiveFunctionValue += x * x;
	return value(objectiveFunctionValue);
}
double get_x() const {
	return _x;
}
private:
double _x;
};
#endif
