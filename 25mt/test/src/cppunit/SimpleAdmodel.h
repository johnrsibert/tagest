#ifndef __SimpleAdmodel_h__
#define __SimpleAdmodel_h__
/**
 * $Id: Simple.h 777 2006-03-11 01:38:06Z johnoel $
 */
#include "AdmodelTemplate.h"

class SimpleAdmodel
	:public AdmodelTemplate
{
public:
SimpleAdmodel():_a(0),_b(0),a(0),b(0) {;}
SimpleAdmodel(const double A, const double B):_a(A),_b(B),a(A),b(B) {;}
virtual ~SimpleAdmodel() {;}

public:
int getTotalVariables() {
	return 2;
}
void initialize(dvector& independantVariables) {
	independantVariables(1) = boundpin(a, -100, 100);
	independantVariables(2) = boundpin(b, -100, 100);
}
double computeObjectiveFunctionValue(dvar_vector independantVariables) {
	dvariable objectiveFunctionValue = 0;

	dvariable penaltyA = 0;
	a = boundp(independantVariables(1),-100,100,penaltyA);
	_a = value(a);
	objectiveFunctionValue += penaltyA;

	dvariable penaltyB = 0;
	b = boundp(independantVariables(2),-100,100,penaltyB);
	_b = value(b);
	objectiveFunctionValue += penaltyB;


	const int nobs = 10;
	dvector Y("{1.4,4.7,5.1,8.3,9.0,14.5,14.0,13.4,19.2,18}");
	dvector x("{-1,0,1,2,3,4,5,6,7,8}");

	dvar_vector pred_Y(1, nobs); pred_Y.initialize();
	pred_Y = a * x + b;

	objectiveFunctionValue = regression(Y, pred_Y);

	return value(objectiveFunctionValue);
}
double get_a() const {
	return _a;
}
double get_b() const {
	return _b;
}
void output() {
	cout << a << ' ' << b << endl;
	cout << _a << ' ' << _b << endl;
}
public:
double _a;
double _b;
dvariable a;
dvariable b;
};
#endif
