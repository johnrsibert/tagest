#ifndef __AdmodelTemplate_h__
#define __AdmodelTemplate_h__
#include <fvar.hpp>
class AdmodelTemplate
{
public:
AdmodelTemplate() {;}
AdmodelTemplate(const AdmodelTemplate&);
virtual ~AdmodelTemplate() {;}

public:
virtual int getTotalVariables() = 0;
virtual void initialize(dvector& independantVariables) = 0;
virtual double computeObjectiveFunctionValue(dvar_vector independantVariables) = 0;
virtual void output() {}
};
#endif
