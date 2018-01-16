#ifndef __MinimizerTemplate_h__
#define __MinimizerTemplate_h__
/**
 * $Id: MinimizerTemplate.h 2737 2010-08-05 22:14:06Z johnoel $
 */
#include <fvar.hpp>
#include "AdmodelTemplate.h"

class MinimizerTemplate 
{
public:
MinimizerTemplate() {;}
MinimizerTemplate(const MinimizerTemplate&);
virtual ~MinimizerTemplate() {;}

public:
double minimize(AdmodelTemplate& admodelTemplate);
dvector get_gradients() const {
	return _gradients;	
}
private:
dvector _gradients;
};
#endif
